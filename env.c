#include "env.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
int _putenv_s(const char *name, const char *value);
#endif

static char *duplicate_string(const char *source, size_t length) {
    char *copy = malloc(length + 1);
    if (copy != NULL) {
        memcpy(copy, source, length);
        copy[length] = '\0';
    }
    return copy;
}

int is_valid_assignment(const char *input) {
    const char *equals = strchr(input, '=');
    if (equals == NULL || equals == input) return 0;
    if (!isalpha((unsigned char)input[0]) && input[0] != '_') return 0;
    for (const char *cursor = input; cursor < equals; cursor++) {
        if (!isalnum((unsigned char)*cursor) && *cursor != '_') return 0;
    }
    return 1;
}

void handle_valid_assignment(const char *input) {
    const char *equals = strchr(input, '=');
    size_t name_length = (size_t)(equals - input);
    char *name = malloc(name_length + 1);
    if (name == NULL) {
        perror("tinyshell: malloc");
        return;
    }
    memcpy(name, input, name_length);
    name[name_length] = '\0';
#ifdef _WIN32
    if (_putenv_s(name, equals + 1) != 0) perror("tinyshell: _putenv_s");
#else
    if (setenv(name, equals + 1, 1) != 0) perror("tinyshell: setenv");
#endif
    free(name);
}

char *expand_variables(const char *input) {
    size_t capacity = strlen(input) + 1;
    char *expanded = malloc(capacity);
    if (expanded == NULL) return NULL;

    size_t output_length = 0;
    for (size_t i = 0; input[i] != '\0'; i++) {
        if (input[i] == '$') {
            size_t start = i + 1;
            size_t length = 0;
            while (isalnum((unsigned char)input[start + length]) ||
                   input[start + length] == '_') length++;
            if (length > 0) {
                char *name = duplicate_string(input + start, length);
                const char *value = name == NULL ? NULL : getenv(name);
                if (name == NULL) {
                    free(expanded);
                    return NULL;
                }
                if (value != NULL) {
                    size_t value_length = strlen(value);
                    while (output_length + value_length + 1 > capacity) {
                        capacity *= 2;
                        char *resized = realloc(expanded, capacity);
                        if (resized == NULL) {
                            free(name);
                            free(expanded);
                            return NULL;
                        }
                        expanded = resized;
                    }
                    memcpy(expanded + output_length, value, value_length);
                    output_length += value_length;
                }
                free(name);
                i += length;
                continue;
            }
        }
        if (output_length + 2 > capacity) {
            capacity *= 2;
            char *resized = realloc(expanded, capacity);
            if (resized == NULL) {
                free(expanded);
                return NULL;
            }
            expanded = resized;
        }
        expanded[output_length++] = input[i];
    }
    expanded[output_length] = '\0';
    return expanded;
}

void print_shell_var(void) {
#ifdef _WIN32
    extern char **_environ;
    char **environment = _environ;
#else
    extern char **environ;
    char **environment = environ;
#endif
    for (char **entry = environment; *entry != NULL; entry++) puts(*entry);
}
