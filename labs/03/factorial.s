.data
n: .word 8

.text
main:
    la t0, n
    lw a0, 0(t0)
    jal ra, factorial
    
    addi a1, a0, 0
    addi a0, x0, 1
    ecall # Print Result
    
    addi a0, x0, 10
    ecall # Exit

factorial:
    addi t0, a0, 0
    addi a0, x0, 1
loop:
	beq t0, x0, end
	mul a0, a0, t0
	addi t0, t0, -1
	j loop
end:
	jr ra
