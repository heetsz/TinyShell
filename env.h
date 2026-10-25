#ifndef TINYSHELL_ENV_H
#define TINYSHELL_ENV_H

int is_valid_assignment(const char *input);
void handle_valid_assignment(const char *input);
char *expand_variables(const char *input);
void print_shell_var(void);

#endif
