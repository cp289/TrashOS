# Provides ARCH
include make.config

NAME=TrashOS

ARCH?=i386
MARCH?=i686
ARCHDIR=kernel/arch/$(ARCH)

CC=$(MARCH)-elf-gcc
AS=$(MARCH)-elf-as
LD=$(MARCH)-elf-ld

# TODO Supposedly x86-64 ABI requires .eh_frame sections, so should
# -fno-asynchronous-unwind-tables be removed or placed into an
#  architecture-specific config?
DEBUG=-g3 -fno-asynchronous-unwind-tables
OPT=-O3 -pipe
LTO=-flto=jobserver
WARN=-pedantic -Wall -Wextra -Werror
ASFLAGS=-march=$(MARCH)
CFLAGS_NOLTO=-std=gnu18 -ffreestanding $(DEBUG) $(OPT) $(WARN)
CFLAGS=$(CFLAGS_NOLTO) $(LTO)
LDFLAGS=-ffreestanding -nostdlib $(DEBUG) $(OPT) $(WARN) -lgcc

KOBJS=\
	$(ARCHDIR)/apic.o \
	$(ARCHDIR)/boot.o \
	$(ARCHDIR)/gdt.o \
	$(ARCHDIR)/init.o \
	$(ARCHDIR)/interrupt.o \
	$(ARCHDIR)/kernel.o \
	$(ARCHDIR)/printk.o \
	$(ARCHDIR)/multiboot2.o \
	$(ARCHDIR)/page.o \
	$(ARCHDIR)/vga.o \

KHDRS=\
	$(ARCHDIR)/apic.h \
	$(ARCHDIR)/asm.h \
	$(ARCHDIR)/cpuid.h \
	$(ARCHDIR)/gdt.h \
	$(ARCHDIR)/interrupt.h \
	$(ARCHDIR)/io.h \
	$(ARCHDIR)/math.h \
	$(ARCHDIR)/mem.h \
	$(ARCHDIR)/page.h \
	$(ARCHDIR)/std.h \
	$(ARCHDIR)/string.h \
	$(ARCHDIR)/vga.h \

KERNEL=$(ARCHDIR)/kernel
ISODIR=iso
ISO=$(NAME).iso

.PHONY: all run clean

# Create implicit rule for linking binaries with ld script prerequisite
%: %.ld
	+$(CC) -T $^ $(LDFLAGS) -o $@

all: $(ISO)

# Header file prerequisites
$(KOBJS): $(KHDRS)

# Special rule for compiling interrupt handlers
$(ARCHDIR)/interrupt.o: $(ARCHDIR)/interrupt.c
	$(CC) -c $< -o $@ $(CPPFLAGS) $(CFLAGS) -mgeneral-regs-only

# Since init.o is placed into a custom section, we must compile without LTO due
# to optimizer bugs. See the following for more details:
#   https://bugs.launchpad.net/gcc-arm-embedded/+bug/1418073
#   https://gcc.gnu.org/bugzilla/show_bug.cgi?id=78903
#   https://gcc.gnu.org/ml/gcc-bugs/2015-03/msg00094.html
$(ARCHDIR)/init.o: $(ARCHDIR)/init.c
	$(CC) -c $< -o $@ $(CPPFLAGS) $(CFLAGS_NOLTO)

$(KERNEL): $(KERNEL).ld $(KOBJS)

$(ISO): $(KERNEL)
	grub-file --is-x86-multiboot2 $^
	mkdir -p $(ISODIR)/boot/grub/
	cp $(KERNEL) $(ISODIR)/boot/
	echo 'menuentry "$(NAME)" { multiboot2 /boot/kernel }' > $(ISODIR)/boot/grub/grub.cfg
	grub-mkrescue -o $@ $(ISODIR)

run: $(ISO)
	qemu-system-$(ARCH) -cdrom $^

guide.pdf: guide.tex
	pdflatex $^

clean:
	rm -f $(KOBJS) $(KERNEL) $(ISO)
	rm -rf $(ISODIR)
