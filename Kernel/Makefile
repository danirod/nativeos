include ../Makefile.inc
CFLAGS += -I../Include/
LDFLAGS += -T ../linker.ld

OBJECTS = ../Boot/bootstrap.o main.o console/vgacon.o

All: kernel.img

kernel.img: $(OBJECTS)
	@echo "$(LD) :: $@"
	@$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

%.o: %.c
	@echo "$(CC) :: $@"
	@$(CC) $(CFLAGS) -o $@ -c $<

%.o: %.s
	@echo "$(AS) :: $@"
	@$(AS) $(ASFLAGS) -o $@ $<

clean:
	rm -f kernel.img $(OBJECTS)
