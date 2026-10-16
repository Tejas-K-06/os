/*
 * xv6-public x86 user program to test the ctxtcnt() system call.
 * Add _ctxtcnt_test to UPROGS in the xv6 Makefile.
 */
#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  printf(1, "Checking timer interrupt and context-switch counters:\n");
  ctxtcnt();

  // Give the timer interrupt a chance to occur, then check again.
  sleep(20);
  printf(1, "After sleep:\n");
  ctxtcnt();

  exit();
}
