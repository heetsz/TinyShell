#ifndef TINYSHELL_HISTORY_H
#define TINYSHELL_HISTORY_H

void init_history(void);
void free_history(void);
void reset_history_index(void);
void add_to_history(const char *command);
const char *get_history_up(void);
const char *get_history_down(void);
void print_history(void);

#endif
