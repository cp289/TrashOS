# Provides ARCH
include make.config

NAME=TrashOS

ARCH?=i386
MARCH?=i686
ARCHDIR=kernel/arch/$(ARCH)

CC=$(MARCH)-elf-gcc
AS=$(MARCH)-elf-as
LD=$(MARCH)-elf-ld

DEBUG=-g0
OPT=-O3 -pipe -flto=jobserver
WARN=-pedantic -Wall -Wextra -Werror
ASFLAGS=-march=$(MARCH)
CFLAGS=-std=gnu18 -ffreestanding $(DEBUG) $(OPT) $(WARN)
LDFLAGS=-ffreestanding -nostdlib $(DEBUG) $(OPT) $(WARN) -lgcc

KOBJS=\
	$(ARCHDIR)/apic.o \
	$(ARCHDIR)/boot.o \
	$(ARCHDIR)/gdt.o \
	$(ARCHDIR)/interrupt.o \
	$(ARCHDIR)/kernel.o \
	$(ARCHDIR)/kprintf.o \
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
