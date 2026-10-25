# TinyShell

TinyShell is a small interactive Unix shell written in C. It is designed as a focused systems-programming project that demonstrates terminal I/O, process creation, environment handling, command history, and built-in commands without relying on a shell framework.

## Features

- Raw terminal input with backspace and arrow-key history navigation
- External command execution with exit status and elapsed-time reporting
- Environment variable assignment and `$VARIABLE` expansion
- Built-ins: `cd`, `pwd`, `printenv`, `print_history`, `help`, and `exit`
- Bounded in-memory history (100 commands)
- Strict compiler warnings and a dependency-free build

## Build and run

TinyShell targets POSIX systems such as macOS and Linux.

```sh
make
./tinyshell
```

Use `help` inside the shell to list built-ins. Press the up/down arrow keys to navigate command history. Run `make clean` to remove the compiled binary.

## Example

```text
tinyshell> NAME=developer
tinyshell> echo Hello $NAME
Hello developer
Exit status: 0
Time taken: 0.842 ms
tinyshell> pwd
/Users/developer/TinyShell
tinyshell> exit
```

## Project structure

| File | Responsibility |
| --- | --- |
| `tinyshell.c` | REPL, built-ins, process execution |
| `env.c/.h` | Assignments, environment listing, variable expansion |
| `history.c/.h` | Bounded command history |
| `rawmode.c/.h` | POSIX terminal mode management |

## Scope

This is intentionally a learning-oriented shell, not a replacement for Bash or Zsh. Quoting, pipelines, redirection, job control, and shell scripting are outside the current scope.
