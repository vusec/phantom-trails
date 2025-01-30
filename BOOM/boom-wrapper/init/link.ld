OUTPUT_ARCH( "riscv" )

SECTIONS
{
  . = 0x80000000;
  .keep_text : { *(.text) }
  . = ALIGN(0x1000);
  .keep_s_data : { *(.s_data) }
  . = ALIGN(0x1000);
  .keep_table1 : { *(.table1) }
  . = ALIGN(0x1000);
  .keep_table2 : { *(.table2) }
  . = ALIGN(0x1000);
  .keep_table3 : { *(.table3) }
  . = ALIGN(0x1000);
  .keep_table2_addr0 : { *(.table2_addr0) }
  . = ALIGN(0x1000);
  .keep_table3_addr0 : { *(.table3_addr0) }
    . = ALIGN(0x1000);
  .keep_empty : { *(.empty) }
  . = ALIGN(0x1000);
  .keep_u_text : { *(.u_text) }
  _end = .;
}
