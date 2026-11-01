# TinyShell

TinyShell is a small interactive Unix shell written in C. It is designed as a focused systems-programming project that demonstrates terminal I/O, process creation, environment handling, command history, and built-in commands without relying on a shell framework.

## Features

- Raw terminal input with backspace and arrow-key history navigation
- External command execution with exit status and elapsed-time reporting
- Environment variable assignment and `$VARIABLE` expansion
- Built-ins: `cd`, `pwd`, `printenv`, `print_history`, `help`, and `exit`
- Bounded in-memory history (100 commands)
- Strict compiler warnings and a dependency-free build

## Requirements

- A C11-compatible compiler such as GCC, Clang, or Apple Clang
- `make`
- A POSIX-compatible environment such as Linux or macOS

## Build and run

Build the shell from the project directory:

```sh
make
./tinyshell
```

The default build uses strict warnings and optimization. To use a different compiler,
pass it to `make`:

```sh
make CC=clang
```

Run `make clean` to remove the compiled binary.

## Usage

Built-in commands:

| Command | Description |
| --- | --- |
| `cd [directory]` | Change the working directory; defaults to `$HOME` |
| `pwd` | Print the current working directory |
| `printenv` | Print the shell environment |
| `print_history` | Print commands from the current session |
| `help` | List available built-ins |
| `exit` | Exit TinyShell |

You can assign environment variables directly and expand them in later commands:

```text
tinyshell> NAME=developer
tinyshell> echo Hello $NAME
Hello developer
```

Use the up and down arrow keys to navigate the in-memory command history, and
Backspace to edit the current command. History is limited to the last 100 commands.

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
| `rawmode.c/.h` | Terminal mode management |

## Scope

TinyShell is intentionally a learning-oriented shell, not a replacement for Bash
or Zsh. Quoting, pipelines, redirection, job control, and shell scripting are
outside the current scope. 
