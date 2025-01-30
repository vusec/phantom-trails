# User mode initialization for MDS.
#
# Perform some operations in user mode to make sure that microarchitectural
# buffers are filled with valid entries before we run the test case.

# This page should not be touched by the rest of the program.
#define FORBIDDEN_ADDR  0x8000C000

.section .u_text , "ax"
init_uarch:
    li s0, FORBIDDEN_ADDR

    .rep 16
    ld t0, 0(s0)
    sd t1, 16(s0)
    addi s0, s0, 32
    .endr

    # Padding for alignment
    nop
