// ********** Derek Leung
// ********** File I/O and parsing - with more features

#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "data.h"
#include "globals.h"

typedef enum {
  END = -1,
  ERR = 0,
  OPAR, CPAR, DOT, STR, QUOT, LIT, BQUOT, COMM
} e_token;

typedef struct {
  e_token type;
  void* data;
} s_token;

s_token make_token(e_token type, void* data) {
  s_token token;
  token.type = type;
  token.data = data;
  return token;
}

struct reader {
  FILE* src;
  int charbuffer;
  int charbvalid;
  s_token tokebuffer;
  int tokebvalid;
  int closed;
  int linenumber;
}; // @heap

struct reader* make_reader(FILE* stream) {
  struct reader* ret = malloc(sizeof(struct reader));
  ret->src = stream;
  ret->charbvalid = 0;
  ret->tokebvalid = 0;
  ret->closed = 0;
  ret->linenumber = 1;
  return ret;
}

int reader_pop(struct reader* r) {
  if (r->charbvalid) {
    r->charbvalid = 0;
    return r->charbuffer;
  }
  int ret = fgetc(r->src);
  if (ret == '\n') {
    r->linenumber++;
  }
  return ret;
}

void reader_push(struct reader* r, int ch) {
  if (r->charbvalid) {
    printf("ERROR: CANNOT PUSH TWICE\n");
    exit(1);
  }
  r->charbvalid = 1;
  r->charbuffer = ch;
}

////////// *** Lexical analysis

int starts_token_str(char c) {
  return ((c >= 'a' && c <= 'z') ||
	  (c >= 'A' && c <= 'Z') ||
	  (c >= '0' && c <= '9') ||
	  strchr("!@#$%^&*-_=+/?<>:", c));
}

int continues_token_str(char c) {
  return starts_token_str(c);
}

int is_whitespace(char c) {
  return strchr(" \t\n", c) != 0;
}

int to_integer(char* tstr) { // tstr = token string
  int num = 0, sign = 1;
  char* cptr = tstr;

  if (*cptr == '-') {
    sign = -1;
    cptr++;
  } else if (*cptr == '+') {
    cptr++;
  }

  char c;
  while ((c = *cptr)) {
    num *= 10;
    num += (c - '0');
    cptr++;
  }

  free(tstr);

  return num * sign;
}

//////////

// @heap
s_token make_token_str(char first, struct reader* r) {
  static char buf[MAX_SYMBOL_SIZE+1]; // +1 for null-term
  int size = 0;
  char c;

  buf[size++] = first;
  while (continues_token_str( (c = reader_pop(r)) )) {
    if (size > MAX_SYMBOL_SIZE) {
      printf("ERROR: SYMBOL TOO LONG\n");
      exit(1);
    }
    buf[size++] = c;
  }
  reader_push(r, c);
  buf[size++] = 0;

  char* name = malloc(sizeof(char) * size);
  for (int i = 0; i < size; i++) {
    name[i] = buf[i];
  }
  return make_token(STR, name);
}

// @heap
s_token make_token_quoted(char first, struct reader* r) {
  static char buf[MAX_SYMBOL_SIZE+1]; // +1 for null-term
  int size = 0;
  char c;

  while ((c = reader_pop(r)) != '"') {
    if (c == -1) {
      printf("ERROR: EOF INSIDE QUOTES\n");
      exit(1);
    }
    if (size > MAX_SYMBOL_SIZE) {
      printf("ERROR: SYMBOL TOO LONG\n");
      exit(1);
    }
    buf[size++] = c;
  }
  buf[size++] = 0;

  char* name = malloc(sizeof(char) * size);
  for (int i = 0; i < size; i++) {
    name[i] = buf[i];
  }
  return make_token(LIT, name);
}

void consume_line(struct reader* r) {
  char c;
  while ((c = reader_pop(r)) != '\n') {
    if (c == -1) {
      reader_push(r, c);
      return;
    }
  }
}

s_token next_token(struct reader* r) {
  int c;
  while ((c = reader_pop(r)) != -1) {
    switch (c) {
    case '(':
      return make_token(OPAR, 0);
    case ')':
      return make_token(CPAR, 0);
    case '\'': // push backwards
      return make_token(QUOT, 0);
    case '`':
      return make_token(BQUOT, 0);
    case ',':
      return make_token(COMM, 0);
    case '.':
      return make_token(DOT, 0);
    case '"':
      return make_token_quoted(c, r);
    case ';':
      consume_line(r);
      break;
    default:
      if (starts_token_str(c)) {
	return make_token_str(c, r);
      } else if (is_whitespace(c)) {
	; // pass
      } else {
	printf("%c\n", c);
	return make_token(ERR, 0);
      }
    }
  }
  return make_token(END, 0);
}

s_token token_pop(struct reader* r) {
  if (r->tokebvalid) {
    r->tokebvalid = 0;
    return r->tokebuffer;
  }
  return next_token(r);
}

void token_push(struct reader* r, s_token t) {
  if (r->tokebvalid) {
    printf("ERROR: DOUBLEPUSH\n");
    exit(1);
  }
  r->tokebuffer = t;
  r->tokebvalid = 1;
}


//////////
////////// *** Parser
//////////

char* make_quotestr(char* quotestr) {
  char* ret = malloc(sizeof(quotestr));
  strcpy(ret, quotestr);
  return ret;
}

struct obj* classify(char* token_str) {
  if (!strcmp("nil", token_str)) {
    return make_object(NIL, 0);
  }

  int isnum = 1; // numerical classifier

  char* cptr = token_str;
  char c;

  if (*cptr == '-' || *cptr == '+') {
    cptr++;
    if (!*cptr) {
      isnum = 0;
    }
  }
  while ((c = *cptr)) {
    if (c < '0' || c > '9') {
      isnum = 0;
    }
    cptr++;
  }

  if (isnum) {
    return make_small_object(NUMBER, to_integer(token_str));
  }
  return make_object(SYMBOL, token_str);
}

struct obj* parse_atom(s_token token) {
  if (token.type == STR) {
    return classify(token.data);
  }
  return make_error("parse_atom called with non-atom");
}

struct obj* next_list(struct reader* r) {
  s_token n = token_pop(r);
  if (n.type == CPAR) {
    return make_object(NIL, 0);
  } else if (n.type == END) {
    return make_error("missing closing parenthesis");
  } else { // TODO error check for bad tokens
    token_push(r, n);
    struct obj* first = next_object(r);
    struct obj* rest;
    n = token_pop(r);
    if (n.type == DOT) {
      rest = next_object(r);
      n = token_pop(r);
      if (n.type != CPAR) {
	return make_error("cell dot (.) construction needs to have a "
			   "closing parenthesis '('");
      }
      return make_object(CELL, make_cell(first, rest));
    } else {
      token_push(r, n);
      return make_object(CELL, make_cell(first, next_list(r)));
    }
  }
}

struct obj* p_next_object(struct reader* r) {
  s_token n = token_pop(r);
  if (n.type == STR) {
    return parse_atom(n);
  } else if (n.type == LIT) {
    return make_object(STRING, n.data);
  } else if (n.type == OPAR) {
    return next_list(r);
  } else if (n.type == END) {
    return make_object(DONE, 0);
  } else if (n.type == QUOT || n.type == BQUOT || n.type == COMM) {
    char* name;
    if (n.type == QUOT) {
      name = "quote";
    } else if (n.type == BQUOT) {
      name = "backquote";
    } else {
      name = "unquote";
    }
    return make_object(CELL, make_cell(parse_atom(make_token(STR, make_quotestr(name))),
				       make_object(CELL, make_cell(next_object(r),
								   make_object(NIL, 0)))));
  } else if (n.type == CPAR) {
    return make_error("missing matching opening parenthesis");
  }
  return make_error("token is invalid");
}

struct obj* next_object(struct reader* r) {
  if (r->closed) {
    return make_error("stream has already been closed");
  }

  struct obj* ret = p_next_object(r);
  if (ret->type == ERROR) { // TODO alloc more smartly
    ret = make_object(ERROR, "parser error in next_object!()");
  }
  return ret;
}

int reader_fclose(struct reader* r) {
  r->closed = 1;
  return fclose(r->src);
}
