# Validation and policy notes

These patches are syntactically valid unified-diff files (`git apply --numstat` parses all 18). They target the classic x86 `mit-pdos/xv6-public` tree after applying `612415074(1).diff_3`. This runtime could not clone GitHub because network DNS was unavailable, so the patches have **not** been tested with `git apply --check` against a full checkout or built in xv6/QEMU. Check against your local clone before use.

Each policy patch is an alternative scheduler; apply only one policy patch after the baseline diff. Do not stack the 18 patches together because they intentionally replace the same scheduler.

- 01 FCFS: FIFO dispatch selection; timer preemption remains active.
- 02 RR: fixed per-process default quantum of 4; no runtime quantum setter.
- 03 priority aging: stored priority is aged in place; lower number is higher priority.
- 04 / 07 priority: lower numeric value is higher, matching the baseline diff.
- 05 SJF and 06 SRTF: default burst estimates are 1; add/set estimates in tests for meaningful differentiation.
- 08 MLFQ: 3 queues, 2/4/8 tick quanta and demotion; periodic boost is not included.
- 09 MLQ: class defaults to 0; no user setter is included.
- 10 lottery: weighted random draw implemented with a kernel-local PRNG; all processes default to one ticket and no ticket setter is included.
- 11 stride: default ticket count 1; no ticket setter or pass normalization is included.
- 12 CFS-inspired: minimum virtual runtime, unweighted and simplified.
- 13 EDF: deadlines default to far future; no deadline setter is included.
- 14 counters: counters are kernel-side only; no user-facing stats syscall is added.
- 15 tests the baseline diff's existing setpriority/getpriority calls; it does not redefine the existing ABI.
- 16 records creation/first timer observation/finish/CPU tick metrics; no user-facing metrics syscall is added.
- 17 hybrid priority aging/FCFS; effective priority is separate from configured priority.
- 18 runtime policy selection adds setschedpolicy syscall 24; it is not intended to be combined with other scheduler patches.

For any patch, if `git apply --check` reports a context mismatch, use the exact checkout/commit expected by the assignment or adjust the hunk context. Classic xv6-public is an old x86 project and needs a compatible 32-bit toolchain/QEMU to build.
