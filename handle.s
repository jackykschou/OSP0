	.file	"handle.c"
	.section	.rodata
.LC0:
	.string	"Nice try.\n"
	.text
	.globl	sigint_handler
	.type	sigint_handler, @function
sigint_handler:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, -20(%rbp)
	movl	$1, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	$10, %edx
	movl	$.LC0, %esi
	movl	%eax, %edi
	call	write
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	sigint_handler, .-sigint_handler
	.section	.rodata
.LC1:
	.string	"Still here\n"
	.text
	.globl	sigalrm_handler
	.type	sigalrm_handler, @function
sigalrm_handler:
.LFB1:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, -20(%rbp)
	movl	$1, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	$11, %edx
	movl	$.LC1, %esi
	movl	%eax, %edi
	call	write
	movl	$1, %edi
	call	alarm
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	sigalrm_handler, .-sigalrm_handler
	.section	.rodata
.LC2:
	.string	"exiting\n"
	.text
	.globl	sigusr1_handler
	.type	sigusr1_handler, @function
sigusr1_handler:
.LFB2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, -20(%rbp)
	movl	$1, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	$9, %edx
	movl	$.LC2, %esi
	movl	%eax, %edi
	call	write
	movl	$0, %edi
	call	exit
	.cfi_endproc
.LFE2:
	.size	sigusr1_handler, .-sigusr1_handler
	.section	.rodata
.LC3:
	.string	"%lld\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB3:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$64, %rsp
	movl	%edi, -52(%rbp)
	movq	%rsi, -64(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movl	$1, -40(%rbp)
	call	getpid
	movl	%eax, -36(%rbp)
	movq	$0, -32(%rbp)
	movq	$0, -24(%rbp)
	movb	$0, -16(%rbp)
	movl	$.LC3, %ecx
	movl	-36(%rbp), %edx
	leaq	-32(%rbp), %rax
	movq	%rcx, %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	sprintf
	leaq	-32(%rbp), %rcx
	movl	-40(%rbp), %eax
	movl	$17, %edx
	movq	%rcx, %rsi
	movl	%eax, %edi
	call	write
	movl	$sigalrm_handler, %esi
	movl	$14, %edi
	call	signal
	movl	$sigint_handler, %esi
	movl	$2, %edi
	call	signal
	movl	$sigusr1_handler, %esi
	movl	$10, %edi
	call	signal
	movl	$1, %edi
	call	alarm
.L5:
	jmp	.L5
	.cfi_endproc
.LFE3:
	.size	main, .-main
	.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
	.section	.note.GNU-stack,"",@progbits
