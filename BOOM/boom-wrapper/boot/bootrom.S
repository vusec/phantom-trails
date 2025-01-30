#define BOOTADDR_REG 0x4000
#define DRAM_BASE 0x80000000

.section .text.start, "ax", @progbits
.globl _start
_start:
  auipc  t0,0x0
  li     t0, DRAM_BASE
  jr     t0

.section .text.hang, "ax", @progbits
.globl _hang
_hang: // reset vector
  auipc  t0,0x0
  li     t0, DRAM_BASE
  jr     t0

.section .text.hang80, "ax", @progbits
.globl _hang80
_hang80:
  auipc  t0,0x0
  li     t0, DRAM_BASE
  jr     t0

.align 3
_dtb:
