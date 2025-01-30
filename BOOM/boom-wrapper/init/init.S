/* PMP configuration */
#define PMP_R                0x01
#define PMP_W                0x02
#define PMP_X                0x04
#define PMP_A                0x18
#define PMP_A_TOR            0x08
#define PMP_A_NA4            0x10
#define PMP_A_NAPOT          0x18
#define PMP_L                0x80

#define CSR_CYCLE            0xc00
#define CSR_TIME             0xc01
#define CSR_MCAUSE           0x342
#define CSR_MTVAL            0x343
#define CSR_MIP              0x344
#define CSR_PMPCFG0          0x3a0
#define CSR_PMPADDR0         0x3b0
#define CSR_MHARTID          0xf14

// ------------------ Machine-Mode Code
.text
init_trap_vector:
    // all traps will end up in the pc
    // being set to trap_handler
    lla t0, trap_handler
    csrw mtvec, t0

change_to_S:
    // clear Trap Virtual Memory(TVM) bit
    li s1, 0x00100000
    csrc mstatus, s1

    // set MPP such that we return to S mode
    li s1, 0x00001000
    csrc mstatus, s1
    li s1, 0x00000800
    csrs mstatus, s1

    // Setup a PMP to permit access to all of memory
    li a0, -1
    csrw CSR_PMPADDR0, a0
    li a0, (PMP_A_NAPOT | PMP_R | PMP_W | PMP_X)
    csrw CSR_PMPCFG0, a0

    // load address of S mode code
    lla s2, s_code
    csrw mepc, s2

    // return to S mode code
    mret

.align 4
trap_handler:
  unimp

// ------------------ Supervisor Code
.align 4
s_code:
set_page_table:
    // set the satp register and page table accordingly
    // page table address is 0x00000080002 (actual address 0x80002000)
    // ASID is 0x0004 (random value)
    // MODE is 0x8 (for Sv39)
    li s0, 0x8000400000080002
    csrw satp, s0

init_cache:
    // Load taint into the cache hierarchy.
    lla s0, s_mem
    addi s0, s0, 8
    ld s0, 0(s0)

prepare_change_to_U:
    // set SPP such that we return to U mode
    li s1, 0x00000100
    csrc sstatus, s1

    // load address of user mode code
    lla s2, u_code
    # li s2, 0x80008000
    csrw sepc, s2

init_regs:
    // Initialize registers
    // Basic regs
    li x1, 0x80008000
    li x2, 0x80008000
    li x3, 0x80008000
    li x4, 0x80008000

    // User data
    li x5, 0x80009000
    li x6, 0x80009000
    li x7, 0x80009000
    li x8, 0x80009000

    // Supervisor data
    li x9,  0x80001000
    li x10, 0x80001000
    li x11, 0x80001000
    li x12, 0x80001000

    // User code
    li x13, 0x8000A000
    li x15, 0x8000A000
    li x15, 0x8000A000
    li x16, 0x8000A000

    // Other user data
    li x17, 0x8000B000
    li x18, 0x8000B000
    li x19, 0x8000B000
    li x20, 0x8000B000

    // Power of two constants
    li x21, 8
    li x22, 16
    li x23, 32
    li x24, 64


change_to_u:
    // return to U mode code
    sret


// ------------------ Supervisor Data
.section .s_data , "adw"
s_mem:
  .dword 0x0000000000000000
  .dword 0x0000000000000000
  .dword 0x0000000000000000
  .dword 0x0000000000000000
  .dword 0x0000000000000000
  .dword 0x0000000000000000
  .dword 0x0000000000000000
  .dword 0x0000000000000000
  .dword 0x0000000000000000
  .dword 0x0000000000000000
  .dword 0x0000000000000000
  .dword 0x0000000000000000
  .dword 0x0000000000000000
  .dword 0x0000000000000000
  .dword 0x0000000000000000
  .dword 0x0000000000000000

// ------------------ Page Tables ---------------------
// table1@0x80002000
.section .table1 , "adw"
table1:
    .dword 0x20001401 // table2_addr0
    .dword 0x0 // empty
    .dword 0x20000c01 // table2

// table2@0x80003000
.section .table2 , "adw"
table2:
    .dword 0x20001001 // table3

// table3@0x80004000
.section .table3 , "adw"
table3:
    .dword 0x2000004b // text
    .dword 0x200004c7 // s_data
    .dword 0x0 // table1
    .dword 0x0 // table2
    .dword 0x0 // table3
    .dword 0x0 // table2_addr0
    .dword 0x0 // table3_addr0
    .dword 0x0 // empty
    .dword 0x2000205b // u_text
    .dword 0x200024d7 // u_data
    .dword 0x200028d7 // u_data
    .dword 0x20002cd7 // u_data
    .dword 0x200030d7 // u_data (MDS)

// table2_addr0@0x80005000
.section .table2_addr0 , "adw"
table2_addr0:
    .dword 0x20001801 // table3_addr0

// table3_addr0@0x80006000
.section .table3_addr0 , "adw"
table3_addr0:
    .dword 0x2000205b // u_text


// empty (for binary alignment)
.section .empty , "ax"
empty:
    .rep 0x200
    .dword 0x0
    .endr

// ---------------- End Page Tables -------------------


// this is the code section for user mode code
.section .u_text , "ax"
u_code:
    // user mode code
