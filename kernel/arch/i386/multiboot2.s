## multiboot2.s: Multiboot 2 header
#
# For documentation, see:
#  https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html

.set magic, 0xE85250D6
.set architecture, 0    # 32-bit protected mode on i386
.set header_length, _multiboot2_end - _multiboot2_start
.set checksum, -(magic + architecture + header_length)

.section .multiboot2

_multiboot2_start:

.int magic
.int architecture
.int header_length
.int checksum

# Termination tag (NOTE "words" are 16 bits)
.word 0
.word 0
.int 0

_multiboot2_end:
