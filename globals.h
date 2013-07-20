// ********** Derek Leung
// ********** Global data objects

#ifndef GLOBALS_H_INCLUDED
#define GLOBALS_H_INCLUDED

#define DEBUG 1

// TODO: this global must be initialized properly somewhere
extern struct reader* stdin_reader;
extern struct table* global_table;

void initialize(void);

#endif
