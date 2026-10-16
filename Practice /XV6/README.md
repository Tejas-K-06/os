# 18 xv6 scheduling patch templates

These files are **patch templates / implementation sketches**, not verified ready-to-apply patches. The `@@` hunk headers are placeholders because xv6 forks differ, and several algorithms require additional logic beyond the short selection snippets. Do not run `patch -p1 < file.patch` expecting them to apply as-is.

Target assumption: classic xv6-public x86. Verify your exact repo before editing.

## Contents
01 FCFS
02 RR with configurable quantum
03 Priority aging
04 Priority convention/tie-breaking
05 SJF
06 SRTF
07 Preemptive priority
08 MLFQ
09 MLQ
10 Lottery
11 Stride
12 Simplified CFS
13 EDF
14 Per-process scheduling statistics
15 Scheduling-parameter system call
16 Scheduling metrics instrumentation
17 FCFS + aging hybrid sketch
18 Policy switch sketch

## Before using
- Inspect `proc.h`, `proc.c`, `trap.c`, `syscall.h`, `syscall.c`, `sysproc.c`, `user.h`, `usys.S`, and `Makefile`.
- Do not duplicate syscall numbers; choose an unused number.
- Protect shared kernel counters with appropriate locks.
- Initialize every new process field in `allocproc()` and consider `fork()` inheritance.
- For FCFS/SJF non-preemptive behavior, do not simply disable all yields: processes must still yield when they block or voluntarily relinquish the CPU.
- For variable quanta/MLFQ, count actual timer ticks per running process and demote only at the specified thresholds.
- Test in QEMU and run `git diff` to create a patch against your own tree.

Some examples use pseudocode comments or snippets that must be integrated into the surrounding scheduler loop; they are included as revision scaffolds rather than submission-ready diffs.
