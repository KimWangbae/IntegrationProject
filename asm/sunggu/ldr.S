		.data

arr:	.word	1
		.word	2
		.word	3
		.word	4
		.word	5

fmt:	.asciz	"r1 = 0x%x\n"

		.text
		.global	main

print : stmfd	sp!, {lr}
		ldr	r0, =fmt
		bl printf
		ldmfd	sp!, {lr}
		mov pc, lr

main:	stmfd	sp!, {lr}
		ldr r3, =arr
		mov r2, #8
		ldr r1, [r3, r2]
		bl print
		mov r0, #0
		ldmfd sp!, {lr}
		mov pc, lr
