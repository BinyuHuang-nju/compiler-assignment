.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text
read:
  li $v0, 4
  la $a0, _prompt
  syscall
  li $v0, 5
  syscall
  jr $ra

write:
  li $v0, 1
  syscall
  li $v0, 4
  la $a0, _ret
  syscall
  move $v0,$0
  jr $ra


main:
  addi $sp, $sp, -4
  sw $fp, 0($sp)
  move $fp, $sp
  addi $sp, $sp, -4
  lw $t0, -4($fp)
  li $t1, 0
  move $t0, $t1
  sw $t0, -4($fp)
  addi $sp, $sp, -4
  lw $t0, -8($fp)
  lw $t1, -4($fp)
  move $t0, $t1
  sw $t0, -8($fp)
  addi $sp, $sp, -4
  lw $t0, -12($fp)
  li $t1, 1
  move $t0, $t1
  sw $t0, -12($fp)
  addi $sp, $sp, -4
  lw $t0, -16($fp)
  lw $t1, -12($fp)
  move $t0, $t1
  sw $t0, -16($fp)
  addi $sp, $sp, -4
  lw $t0, -20($fp)
  li $t1, 0
  move $t0, $t1
  sw $t0, -20($fp)
  addi $sp, $sp, -4
  lw $t0, -24($fp)
  lw $t1, -20($fp)
  move $t0, $t1
  sw $t0, -24($fp)
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal read
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  addi $sp, $sp, -4
  lw $t0, -28($fp)
  move $t0, $v0
  sw $t0, -28($fp)
  addi $sp, $sp, -4
  lw $t0, -32($fp)
  lw $t1, -28($fp)
  move $t0, $t1
  sw $t0, -32($fp)
label1:
  addi $sp, $sp, -4
  lw $t0, -36($fp)
  lw $t1, -24($fp)
  move $t0, $t1
  sw $t0, -36($fp)
  addi $sp, $sp, -4
  lw $t0, -40($fp)
  lw $t1, -32($fp)
  move $t0, $t1
  sw $t0, -40($fp)
  lw $t0, -36($fp)
  lw $t1, -40($fp)
  bge $t0, $t1, label2
  addi $sp, $sp, -4
  lw $t0, -44($fp)
  lw $t1, -8($fp)
  move $t0, $t1
  sw $t0, -44($fp)
  addi $sp, $sp, -4
  lw $t0, -48($fp)
  lw $t1, -16($fp)
  move $t0, $t1
  sw $t0, -48($fp)
  addi $sp, $sp, -4
  lw $t0, -52($fp)
  lw $t1, -44($fp)
  lw $t2, -48($fp)
  add $t0, $t1, $t2
  sw $t0, -52($fp)
  addi $sp, $sp, -4
  lw $t0, -56($fp)
  lw $t1, -52($fp)
  move $t0, $t1
  sw $t0, -56($fp)
  addi $sp, $sp, -4
  lw $t0, -60($fp)
  lw $t1, -16($fp)
  move $t0, $t1
  sw $t0, -60($fp)
  lw $t0, -60($fp)
  move $a0, $t0
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  addi $sp, $sp, -4
  lw $t0, -64($fp)
  lw $t1, -16($fp)
  move $t0, $t1
  sw $t0, -64($fp)
  lw $t0, -8($fp)
  lw $t1, -64($fp)
  move $t0, $t1
  sw $t0, -8($fp)
  addi $sp, $sp, -4
  lw $t0, -68($fp)
  lw $t1, -56($fp)
  move $t0, $t1
  sw $t0, -68($fp)
  lw $t0, -16($fp)
  lw $t1, -68($fp)
  move $t0, $t1
  sw $t0, -16($fp)
  addi $sp, $sp, -4
  lw $t0, -72($fp)
  lw $t1, -24($fp)
  move $t0, $t1
  sw $t0, -72($fp)
  addi $sp, $sp, -4
  lw $t0, -76($fp)
  li $t1, 1
  move $t0, $t1
  sw $t0, -76($fp)
  addi $sp, $sp, -4
  lw $t0, -80($fp)
  lw $t1, -72($fp)
  lw $t2, -76($fp)
  add $t0, $t1, $t2
  sw $t0, -80($fp)
  lw $t0, -24($fp)
  lw $t1, -80($fp)
  move $t0, $t1
  sw $t0, -24($fp)
  j label1
label2:
  addi $sp, $sp, -4
  lw $t0, -84($fp)
  li $t1, 0
  move $t0, $t1
  sw $t0, -84($fp)
  lw $t0, -84($fp)
  move $v0, $t0
  move $sp, $fp
  lw $fp, 0($sp)
  addi $sp, $sp, 4
  jr $ra
