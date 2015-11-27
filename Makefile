include Makefile.inc

# Default rule: build the kernel.
default: Kernel/kernel.img

# Extra rule: generate CD image.
iso: NativeOS.iso

# This rule executes NativeOS using qemu.
qemu: NativeOS.iso
	qemu-system-i386 -cdrom NativeOS.iso

NativeOS.iso: Kernel/kernel.img
	cp Kernel/kernel.img ISO/boot/kernel.img
	genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot \
		-boot-load-size 4 -A os -input-charset utf8 -quiet \
		-boot-info-table -o NativeOS.iso ISO

Kernel/kernel.img:
	@make -C Kernel

clean:
	@make clean -C Kernel
	rm -f ISO/boot/kernel.img
	rm -f NativeOS.iso
