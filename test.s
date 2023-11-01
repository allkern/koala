
unopt_main:
    move    fp, sp
    addi    sp, -12
    stl.w   8(sp), ra
    li      t0, 1
    stl.w   0(sp), t0
    li      t0, 2
    ldl.w   t1, 0(sp)
    add     t0, t0, t1
    stl.w   4(sp), t0
    ldl.w   t0, 0(sp)
    ldl.w   t1, 4(sp)
    add     v0, t0, t1
    ldl.w   ra, 8(sp)
    jalr    ra, r0
    move    sp, fp

opt_main:
    move    fp, sp
    addi    sp, -4
    stl.w   0(sp), ra
    li      t0, 1
    li      t1, 2
    add     t1, t1, t0
    add     v0, t0, t1
    ldl.w   ra, 0(sp)
    jalr    ra, r0
    move    sp, fp

O3_main:
    li      a0, 4
    jalr    ra, r0
    nop