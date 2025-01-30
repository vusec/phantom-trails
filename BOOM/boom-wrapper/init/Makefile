RISCV_PREFIX ?= /chipyard/.conda-env/riscv-tools/bin/riscv64-unknown-elf-
RISCV_GCC ?= $(RISCV_PREFIX)gcc
RISCV_GCC_OPTS ?= -O0 -static -mcmodel=medany -fvisibility=hidden -nostdlib -nostartfiles -fno-pic -march=rv64g
RISCV_OBJDUMP ?= $(RISCV_PREFIX)objdump -b binary -m riscv:rv64 -M no-aliases -D -z
RISCV_OBJCOPY ?= $(RISCV_PREFIX)objcopy -O binary --only-section=.keep*

# Programs to compile
SOURCES = $(wildcard ./*.S)

DUMPS = $(patsubst ./%.S,./%.asm,$(SOURCES))
BINS =  $(patsubst ./%.S,./%.bin,$(SOURCES))

.PHONY: all

all: $(BINS) $(DUMPS)

%.bin: %.riscv
	$(RISCV_OBJCOPY) $< $@

# %.hex: %.riscv
# 	/chipyard/scripts/smartelf2hex.sh $< > $@

%.asm: %.bin
	$(RISCV_OBJDUMP) $< > $@

%.riscv: %.S
	$(RISCV_GCC) $(RISCV_GCC_OPTS) -I. -T./link.ld $< -o $@


.PHONY: clean

clean:
	rm -f *.bin *.hex *.riscv
