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
	$(ARCHDIR)/boot.o \
	$(ARCHDIR)/gdt.o \
	$(ARCHDIR)/gdt_load.o \
	$(ARCHDIR)/kernel.o \
	$(ARCHDIR)/multiboot2.o \
	$(ARCHDIR)/register.o \
	$(ARCHDIR)/vga.o \

KHDRS=\
	$(ARCHDIR)/gdt.h \
	$(ARCHDIR)/io.h \
	$(ARCHDIR)/math.h \
	$(ARCHDIR)/register.h \
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
$(ARCHDIR)/gdt.o: $(KHDRS)
$(ARCHDIR)/kernel.o: $(KHDRS)
$(ARCHDIR)/vga.o: $(KHDRS)

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
