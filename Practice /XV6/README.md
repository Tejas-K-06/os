# xv6-public scheduling patches (18 alternatives)

Target: `https://github.com/mit-pdos/xv6-public` (classic x86 xv6-public).

## Apply

1. Start from a clean checkout of `mit-pdos/xv6-public` master.
2. Apply the supplied baseline diff first: `git apply 612415074(1).diff_3`.
3. Apply **one** policy patch at a time, e.g. `git apply 01_fcfs.patch`. These are alternative scheduler implementations and generally cannot be stacked.
4. Build with the xv6 x86 toolchain (`make`); the classic repository requires 32-bit x86 toolchain/QEMU support.

The existing diff adds `setpriority(int)` / `getpriority(void)` as syscall 22/23 and defines priority 1 as default. Patches 03/04/07/17 use 1=highest and 5=lowest.

## Honest validation status

These files were generated against the repository's known `proc.c`/`proc.h`/`trap.c` structure and the uploaded baseline diff, but this runtime could not fetch a full GitHub checkout (network DNS unavailable), so **they have not been run through `git apply --check` or a kernel build against the full repository**. Review `POLICY_NOTES.md` before applying: some advanced templates need extra API/test work beyond a selection-only scheduler.

## Patch numbering

01 FCFS; 02 variable RR; 03 priority aging; 04 priority convention; 05 SJF; 06 SRTF; 07 preemptive priority; 08 MLFQ; 09 MLQ; 10 lottery; 11 stride; 12 simplified CFS; 13 EDF; 14 scheduling stats; 15 existing scheduling parameter syscall test; 16 metrics; 17 FCFS+aging hybrid; 18 runtime policy selection.
