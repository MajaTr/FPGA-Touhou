divrem:
	addi sp,sp,-32	# Allocate stack space
	
	# store any callee-saved register you might overwrite
	sw ra, 0(sp)

	
	li t0, 0
	li t1, 0
	li t2, 0
	li t5, 32
	indexloop:
		addi t2, t2, 1
		beq t2, t5, break
		srl t3, a0, t2
	bne t3, zero, indexloop	
	break:
	
	li t4, -1
	loopstart:
		add t2, t2, t4
		slli t0, t0, 1
		slli t1, t1, 1
		srl t3, a0, t2
		andi t3, t3, 1
		add t1, t1, t3
		blt t1, a1, less
			sub t1, t1, a1
			addi t0, t0, 1 
		less:
	bne t2, zero, loopstart
	mv a0, t0
	mv a1, t1
	
	
	# load every register you stored above
	lw ra, 0(sp)
	addi sp,sp,32 	# Free up stack space
	ret

.section .text
.global div
div: 
	addi sp,sp,-32	# Allocate stack space
	
	# store any callee-saved register you might overwrite
	sw ra, 0(sp)

	call divrem

	# load every register you stored above
	lw ra, 0(sp)
	addi sp,sp,32 	# Free up stack space
	ret

.section .text
.global rem
rem:
	addi sp,sp,-32	# Allocate stack space
	
	# store any callee-saved register you might overwrite
	sw ra, 0(sp)

	call divrem
	mv a0, a1

	# load every register you stored above
	lw ra, 0(sp)
	addi sp,sp,32 	# Free up stack space
	ret



	
