# Provides ARCH
include make.config

NAME=TrashOS

ARCH?=i386
MARCH?=i686
ARCHDIR=kernel/arch/$(ARCH)

CC=$(MARCH)-elf-gcc
AS=$(MARCH)-elf-as
LD=$(MARCH)-elf-ld

OPT=-O3 -pipe -flto=jobserver
WARN=-pedantic -Wall -Wextra -Werror
ASFLAGS=-march=$(MARCH)
CFLAGS=-std=gnu18 -ffreestanding $(OPT) $(WARN)
LDFLAGS=-ffreestanding -nostdlib $(OPT) $(WARN) -lgcc

KOBJS=\
	$(ARCHDIR)/boot.o \
	$(ARCHDIR)/kernel.o \
	$(ARCHDIR)/multiboot2.o \
	$(ARCHDIR)/vga.o \

KHDRS=\
	$(ARCHDIR)/io.h \
	$(ARCHDIR)/math.h \
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
