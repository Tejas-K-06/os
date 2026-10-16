# Fixed Pipe and Redirection Programs

This archive contains the 20 corrected C programs, keeping every original `.c` filename unchanged.

## Build
Compile each program from this directory with a C compiler, for example:

```sh
gcc -std=c11 -Wall -Wextra -pedantic -o shell 01_shell_multi_pipe.c
```

Each file's opening comment includes its purpose and, where applicable, usage examples. Most programs are POSIX C programs intended for Linux/macOS.

## Notes
- Pipeline programs close unused pipe descriptors and check `pipe`, `fork`, `dup2`, `exec`, and `waitpid` failures where relevant.
- Child processes use `_exit()` after failures following `fork()`.
- Programs return meaningful nonzero statuses for errors and failed commands where practical.
- The small interactive shells and command-string pipeline examples use a deliberately simple whitespace parser. They do not implement full shell quoting, variable expansion, globbing, or escaping.
