#include "rawmode.h"

#ifdef _WIN32

int enable_raw_mode(void) {
    return 0;
}

void disable_raw_mode(void) {
}

#else

#include <stdio.h>
#include <termios.h>
#include <unistd.h>

static struct termios original_terminal;
static int raw_mode_enabled;

int enable_raw_mode(void) {
    if (!isatty(STDIN_FILENO)) return 0;
    if (tcgetattr(STDIN_FILENO, &original_terminal) != 0) return -1;
    struct termios raw = original_terminal;
    raw.c_lflag &= (tcflag_t) ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) != 0) return -1;
    raw_mode_enabled = 1;
    return 0;
}

void disable_raw_mode(void) {
    if (raw_mode_enabled) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_terminal);
        raw_mode_enabled = 0;
    }
}

#endif
