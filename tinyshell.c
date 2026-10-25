#include "env.h"
#include "history.h"
#include "rawmode.h"

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <io.h>
#define read _read
#define STDIN_FILENO 0
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif
#else
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

#define MAX_INPUT 1024
#define MAX_ARGS 64

static int read_input(char *buffer, size_t capacity) {
    size_t length = 0;
    buffer[0] = '\0';
    reset_history_index();

    if (enable_raw_mode() != 0) {
        return -1;
    }

    for (;;) {
        char character;
        ssize_t bytes_read = read(STDIN_FILENO, &character, 1);

        if (bytes_read == 0) {
            disable_raw_mode();
            return 0;
        }
        if (bytes_read < 0) {
            if (errno == EINTR) continue;
            disable_raw_mode();
            return -1;
        }

        if (character == '\n' || character == '\r') {
            buffer[length] = '\0';
            putchar('\n');
            break;
        }
        if (character == 127 || character == '\b') {
            if (length > 0) {
                buffer[--length] = '\0';
                fputs("\b \b", stdout);
                fflush(stdout);
            }
            continue;
        }
        if (character == 27) {
            char sequence[2];
            if (read(STDIN_FILENO, &sequence[0], 1) == 1 &&
                read(STDIN_FILENO, &sequence[1], 1) == 1 &&
                sequence[0] == '[') {
                const char *command = NULL;
                if (sequence[1] == 'A') command = get_history_up();
                if (sequence[1] == 'B') command = get_history_down();
                if (command != NULL) {
                    printf("\r\033[Ktinyshell> %s", command);
                    length = snprintf(buffer, capacity, "%s", command);
                    fflush(stdout);
                } else if (sequence[1] == 'B') {
                    printf("\r\033[Ktinyshell> ");
                    length = 0;
                    buffer[0] = '\0';
                    fflush(stdout);
                }
            }
            continue;
        }
        if (length < capacity - 1 && character >= 32) {
            buffer[length++] = character;
            buffer[length] = '\0';
            putchar(character);
            fflush(stdout);
        }
    }

    disable_raw_mode();
    return 1;
}

static int execute_external(char *const args[]) {
#ifdef _WIN32
    char command[MAX_INPUT] = "";
    for (size_t i = 0; args[i] != NULL; i++) {
        if (i > 0) strncat(command, " ", sizeof(command) - strlen(command) - 1);
        strncat(command, args[i], sizeof(command) - strlen(command) - 1);
    }
    int status = system(command);
    if (status == -1) {
        perror("tinyshell: system");
        return 1;
    }
    printf("Exit status: %d\n", status);
    return status;
#else
    struct timeval start, end;
    gettimeofday(&start, NULL);
    pid_t child = fork();

    if (child == 0) {
        execvp(args[0], args);
        fprintf(stderr, "tinyshell: %s: %s\n", args[0], strerror(errno));
        _exit(127);
    }
    if (child < 0) {
        perror("tinyshell: fork");
        return 1;
    }

    int status;
    if (waitpid(child, &status, 0) < 0) {
        perror("tinyshell: waitpid");
        return 1;
    }
    gettimeofday(&end, NULL);

    long seconds = end.tv_sec - start.tv_sec;
    long microseconds = end.tv_usec - start.tv_usec;
    double elapsed_ms = seconds * 1000.0 + microseconds / 1000.0;
    int exit_status = WIFEXITED(status) ? WEXITSTATUS(status) : 128 + WTERMSIG(status);

    printf("Exit status: %d\nTime taken: %.3f ms\n", exit_status, elapsed_ms);
    return exit_status;
#endif
}

static void shell_loop(void) {
    char input[MAX_INPUT];
    char *args[MAX_ARGS];
    init_history();

    for (;;) {
        fputs("tinyshell> ", stdout);
        fflush(stdout);

        int input_status = read_input(input, sizeof(input));
        if (input_status <= 0) {
            if (input_status == 0) putchar('\n');
            break;
        }
        if (input[0] == '\0') continue;
        add_to_history(input);

        if (strcmp(input, "exit") == 0) break;
        if (strcmp(input, "printenv") == 0) {
            print_shell_var();
            continue;
        }
        if (strcmp(input, "print_history") == 0) {
            print_history();
            continue;
        }
        if (strcmp(input, "help") == 0) {
            puts("Built-ins: cd, pwd, printenv, print_history, help, exit");
            continue;
        }
        if (is_valid_assignment(input)) {
            handle_valid_assignment(input);
            continue;
        }

        char *expanded_input = expand_variables(input);
        if (expanded_input == NULL) {
            fprintf(stderr, "tinyshell: unable to expand variables\n");
            continue;
        }

        size_t argument_count = 0;
        char *token = strtok(expanded_input, " \t");
        while (token != NULL && argument_count < MAX_ARGS - 1) {
            args[argument_count++] = token;
            token = strtok(NULL, " \t");
        }
        args[argument_count] = NULL;
        if (argument_count == 0) {
            free(expanded_input);
            continue;
        }

        if (strcmp(args[0], "cd") == 0) {
            const char *directory = args[1] != NULL ? args[1] : getenv("HOME");
            if (directory == NULL || chdir(directory) != 0)
                perror("tinyshell: cd");
        } else if (strcmp(args[0], "pwd") == 0) {
            char current_directory[PATH_MAX];
            if (getcwd(current_directory, sizeof(current_directory)) != NULL)
                puts(current_directory);
            else
                perror("tinyshell: pwd");
        } else {
            execute_external(args);
        }
        free(expanded_input);
    }
    free_history();
}

int main(void) {
    shell_loop();
    return EXIT_SUCCESS;
}
