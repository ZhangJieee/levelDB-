	.file	"testLevelDb.cpp"
	.intel_syntax noprefix
	.text
	.p2align 4,,15
	.globl	_Z11thread1FuncPv
	.type	_Z11thread1FuncPv, @function
_Z11thread1FuncPv:
.LFB2137:
	.cfi_startproc
	sub	rsp, 8
	.cfi_def_cfa_offset 16
	.p2align 4,,10
	.p2align 3
.L4:
	lea	rdi, beginSema1[rip]
	call	sem_wait@PLT
	.p2align 4,,10
	.p2align 3
.L2:
	call	rand@PLT
	test	al, 7
	jne	.L2
	mov	DWORD PTR X[rip], 1
	mov	edx, 1
	mfence
	.p2align 4,,10
	.p2align 3
.L3:
	mov	eax, edx
	xchg	eax, DWORD PTR lock[rip]
	test	eax, eax
	jne	.L3
	mov	eax, DWORD PTR Y[rip]
	lea	rdi, endSema[rip]
	mov	DWORD PTR r1[rip], eax
	mov	DWORD PTR lock[rip], 0
	call	sem_post@PLT
	jmp	.L4
	.cfi_endproc
.LFE2137:
	.size	_Z11thread1FuncPv, .-_Z11thread1FuncPv
	.p2align 4,,15
	.globl	_Z11thread2FuncPv
	.type	_Z11thread2FuncPv, @function
_Z11thread2FuncPv:
.LFB2138:
	.cfi_startproc
	sub	rsp, 8
	.cfi_def_cfa_offset 16
	.p2align 4,,10
	.p2align 3
.L12:
	lea	rdi, beginSema2[rip]
	call	sem_wait@PLT
	.p2align 4,,10
	.p2align 3
.L10:
	call	rand@PLT
	test	al, 7
	jne	.L10
	mov	DWORD PTR Y[rip], 1
	mov	edx, 1
	mfence
	.p2align 4,,10
	.p2align 3
.L11:
	mov	eax, edx
	xchg	eax, DWORD PTR lock[rip]
	test	eax, eax
	jne	.L11
	mov	eax, DWORD PTR X[rip]
	lea	rdi, endSema[rip]
	mov	DWORD PTR r1[rip], eax
	mov	DWORD PTR lock[rip], 0
	call	sem_post@PLT
	jmp	.L12
	.cfi_endproc
.LFE2138:
	.size	_Z11thread2FuncPv, .-_Z11thread2FuncPv
	.p2align 4,,15
	.globl	_Z6workerPv
	.type	_Z6workerPv, @function
_Z6workerPv:
.LFB2135:
	.cfi_startproc
	mov	edx, 1
	.p2align 4,,10
	.p2align 3
.L17:
	mov	eax, edx
	xchg	eax, DWORD PTR lock[rip]
	test	eax, eax
	jne	.L17
	mov	DWORD PTR lock[rip], 0
	ret
	.cfi_endproc
.LFE2135:
	.size	_Z6workerPv, .-_Z6workerPv
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC0:
	.string	"%d reorders detected after %d iterations\n"
	.section	.text.startup,"ax",@progbits
	.p2align 4,,15
	.globl	main
	.type	main, @function
main:
.LFB2139:
	.cfi_startproc
	push	r12
	.cfi_def_cfa_offset 16
	.cfi_offset 12, -16
	push	rbp
	.cfi_def_cfa_offset 24
	.cfi_offset 6, -24
	lea	rdx, _Z11thread1FuncPv[rip]
	push	rbx
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -32
	xor	ecx, ecx
	xor	esi, esi
	mov	ebx, 1
	xor	r12d, r12d
	lea	rbp, endSema[rip]
	sub	rsp, 16
	.cfi_def_cfa_offset 48
	mov	rdi, rsp
	call	pthread_create@PLT
	lea	rdi, 8[rsp]
	lea	rdx, _Z11thread2FuncPv[rip]
	xor	ecx, ecx
	xor	esi, esi
	call	pthread_create@PLT
	jmp	.L21
	.p2align 4,,10
	.p2align 3
.L20:
	add	ebx, 1
	cmp	ebx, 10000001
	je	.L24
.L21:
	lea	rdi, beginSema1[rip]
	mov	DWORD PTR X[rip], 0
	mov	DWORD PTR Y[rip], 0
	call	sem_post@PLT
	lea	rdi, beginSema2[rip]
	call	sem_post@PLT
	mov	rdi, rbp
	call	sem_wait@PLT
	mov	rdi, rbp
	call	sem_wait@PLT
	mov	eax, DWORD PTR r1[rip]
	test	eax, eax
	jne	.L20
	add	r12d, 1
	lea	rdi, .LC0[rip]
	mov	edx, ebx
	xor	eax, eax
	mov	esi, r12d
	add	ebx, 1
	call	printf@PLT
	cmp	ebx, 10000001
	jne	.L21
.L24:
	add	rsp, 16
	.cfi_def_cfa_offset 32
	xor	eax, eax
	pop	rbx
	.cfi_def_cfa_offset 24
	pop	rbp
	.cfi_def_cfa_offset 16
	pop	r12
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE2139:
	.size	main, .-main
	.p2align 4,,15
	.type	_GLOBAL__sub_I_lock, @function
_GLOBAL__sub_I_lock:
.LFB2597:
	.cfi_startproc
	lea	rdi, _ZStL8__ioinit[rip]
	sub	rsp, 8
	.cfi_def_cfa_offset 16
	call	_ZNSt8ios_base4InitC1Ev@PLT
	mov	rdi, QWORD PTR _ZNSt8ios_base4InitD1Ev@GOTPCREL[rip]
	lea	rdx, __dso_handle[rip]
	lea	rsi, _ZStL8__ioinit[rip]
	add	rsp, 8
	.cfi_def_cfa_offset 8
	jmp	__cxa_atexit@PLT
	.cfi_endproc
.LFE2597:
	.size	_GLOBAL__sub_I_lock, .-_GLOBAL__sub_I_lock
	.section	.init_array,"aw"
	.align 8
	.quad	_GLOBAL__sub_I_lock
	.globl	IsPublished
	.bss
	.align 4
	.type	IsPublished, @object
	.size	IsPublished, 4
IsPublished:
	.zero	4
	.globl	Value
	.align 4
	.type	Value, @object
	.size	Value, 4
Value:
	.zero	4
	.globl	r1
	.align 4
	.type	r1, @object
	.size	r1, 4
r1:
	.zero	4
	.globl	Y
	.align 4
	.type	Y, @object
	.size	Y, 4
Y:
	.zero	4
	.globl	X
	.align 4
	.type	X, @object
	.size	X, 4
X:
	.zero	4
	.globl	endSema
	.align 32
	.type	endSema, @object
	.size	endSema, 32
endSema:
	.zero	32
	.globl	beginSema2
	.align 32
	.type	beginSema2, @object
	.size	beginSema2, 32
beginSema2:
	.zero	32
	.globl	beginSema1
	.align 32
	.type	beginSema1, @object
	.size	beginSema1, 32
beginSema1:
	.zero	32
	.globl	lock
	.align 4
	.type	lock, @object
	.size	lock, 4
lock:
	.zero	4
	.local	_ZStL8__ioinit
	.comm	_ZStL8__ioinit,1,1
	.hidden	__dso_handle
	.ident	"GCC: (Debian 6.3.0-18+deb9u1) 6.3.0 20170516"
	.section	.note.GNU-stack,"",@progbits
