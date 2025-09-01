.PHONY: all clean

PREFIX	?= arm-none-eabi
CC		= $(PREFIX)-gcc
LD		= $(PREFIX)-gcc
OBJCOPY	= $(PREFIX)-objcopy
OBJDUMP	= $(PREFIX)-objdump
GDB		= $(PREFIX)-gdb

OPENCM3DIR = ../libopencm3
ARMNONEEABIDIR = /usr/bin
COMMONDIR = ../common

CRYPTO_SRC = \
	../pqov/utils/aes128_4r_ffs.c \
	B2M.c \
	blas_masked.c \
	blas_matrix_masked.c \
	bool_util.c \
	fips202_masked.c \
	../pqov/utils/fips202.c \
	gf256_util.c \
	mask_util.c \
	ov_keypair_masked.c \
	ov_masked.c \
	parallel_matrix_op_masked.c \
	refresh.c \
	sign_masked.c \
	utils_hash_masked.c \
	utils_hash.c \
	../pqov/utils/utils_prng.c 
	
all: muov_m4.bin 

muov_m4.%: ARCH_FLAGS = -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16
muov_m4.o: CFLAGS += -DSTM32F4
$(COMMONDIR)/stm32f4_wrapper.o: CFLAGS += -DSTM32F4
muov_m4.elf: LDSCRIPT = $(COMMONDIR)/stm32f407x6.ld
muov_m4.elf: LDFLAGS += -lopencm3_stm32f4
muov_m4.elf: OBJS += $(COMMONDIR)/stm32f4_wrapper.o
muov_m4.elf: $(COMMONDIR)/stm32f4_wrapper.o

CFLAGS		+= -O3 \
		   -Wall -Wextra -Wimplicit-function-declaration \
		   -Wredundant-decls -Wmissing-prototypes -Wstrict-prototypes \
		   -Wundef -Wshadow \
		   -I$(ARMNONEEABIDIR)/include -I$(OPENCM3DIR)/include \
		   -fno-common $(ARCH_FLAGS) -MD \
		   -fno-schedule-insns -fno-schedule-insns2 
LDFLAGS		+= --static -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group \
		   -T$(LDSCRIPT) -nostartfiles -Wl,--gc-sections,--no-print-gc-sections \
		   $(ARCH_FLAGS) -L$(OPENCM3DIR)/lib

OBJS		+= $(CRYPTO_SRC:.c=.o) mask_util.s

%.bin: %.elf
	$(OBJCOPY) -Obinary $^ $@

%.elf: %.o $(OBJS) $(LDSCRIPT)
	$(LD) -o $@ $< $(OBJS) $(LDFLAGS)

muov%.o: main.c 
	$(CC) $(CFLAGS) -o $@ -c $^

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f *.o *.d *.elf *.bin *.hex
