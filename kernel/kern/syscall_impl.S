// int syscall_open(const char *path, int mode);
// Int 0x3F - EAX=1 EBX=path ECX=mode EAX=ret
.global syscall_open
syscall_open:
	// Prepare the stack for this local function.
	push %ebp
	mov %esp, %ebp

	// Call the syscall
	mov $1, %eax
	movl 8(%ebp), %ebx
	movl 12(%ebp), %ecx
	int $0x3C

	// Finish execution
	pop %ebp
	ret

// int syscall_close(const char *path, int mode);
// Int 0x3F - EAX=2 EBX=fd
.global syscall_close
syscall_close:
	// Prepare the stack for this local function.
	push %ebp
	mov %esp, %ebp

	// Call the syscall
	mov $2, %eax
	movl 8(%ebp), %ebx
	int $0x3C

	// Finish execution
	pop %ebp
	ret

// int syscall_close(const char *path, int mode);
// Int 0x3F - EAX=3 EBX=fd ECX=buf EDX=len
.global syscall_read
syscall_read:
	// Prepare the stack for this local function.
	push %ebp
	mov %esp, %ebp

	// Call the syscall
	mov $3, %eax
	movl 8(%ebp), %ebx
	movl 12(%ebp), %ecx
	movl 16(%ebp), %edx
	int $0x3C

	// Finish execution
	pop %ebp
	ret

// int syscall_close(const char *path, int mode);
// Int 0x3F - EAX=4 EBX=fd ECX=buf EDX=len
.global syscall_write
syscall_write:
	// Prepare the stack for this local function.
	push %ebp
	mov %esp, %ebp

	// Call the syscall
	mov $4, %eax
	movl 8(%ebp), %ebx
	movl 12(%ebp), %ecx
	movl 16(%ebp), %edx
	int $0x3C

	// Finish execution
	pop %ebp
	ret
