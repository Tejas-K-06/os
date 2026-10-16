# OS Lab: 20 pipe()/system-call practice programs

These are independent Linux/POSIX C examples. They are practice variations, not predictions of the actual exam.

## Compile
For a file:
```bash
gcc -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L 01_shell_multi_pipe.c -o program
```
Some examples require command-line arguments; each source comment includes a usage example where relevant.

## Important limitations
- The mini-shell parsers split on whitespace; they do not implement shell quoting, escaping, variable expansion, or globbing.
- Examples using `grep`, `cat`, `wc`, `head`, `sort`, or `ls` require those utilities to be installed and available on PATH.
- The redirection examples are intentionally simple teaching examples.
- Always check system-call return values in a production program; the exam may require more extensive error handling.
