#include "history.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HISTORY_CAPACITY 100

static char *entries[HISTORY_CAPACITY];
static size_t entry_count;
static size_t history_index;

static char *duplicate_string(const char *source) {
    size_t length = strlen(source);
    char *copy = malloc(length + 1);
    if (copy != NULL) memcpy(copy, source, length + 1);
    return copy;
}

void init_history(void) {
    entry_count = 0;
    history_index = 0;
}

void free_history(void) {
    for (size_t i = 0; i < entry_count; i++) free(entries[i]);
    entry_count = 0;
}

void reset_history_index(void) {
    history_index = entry_count;
}

void add_to_history(const char *command) {
    if (entry_count == HISTORY_CAPACITY) {
        free(entries[0]);
        memmove(entries, entries + 1, (HISTORY_CAPACITY - 1) * sizeof(entries[0]));
        entry_count--;
    }
    entries[entry_count++] = duplicate_string(command);
}

const char *get_history_up(void) {
    if (entry_count == 0) return NULL;
    if (history_index > 0) history_index--;
    return entries[history_index];
}

const char *get_history_down(void) {
    if (history_index + 1 < entry_count) return entries[++history_index];
    history_index = entry_count;
    return NULL;
}

void print_history(void) {
    for (size_t i = 0; i < entry_count; i++) printf("%zu  %s\n", i + 1, entries[i]);
}
