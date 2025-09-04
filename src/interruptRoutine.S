.extern _ZN5RiscV16interruptHandlerEv

.align 4 # posto u stvec treba da bude 00 na najniza dva bita da bi bila samo jedna prekidna rutina
.global _ZN5RiscV16interruptRoutineEv
.type _ZN5RiscV16interruptRoutineEv, @function
_ZN5RiscV16interruptRoutineEv:
    addi sp, sp, -256
    .irp ind,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    sd x\ind, \ind*8(sp)
    .endr

    call _ZN5RiscV16interruptHandlerEv

    .irp ind,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    ld x\ind, \ind*8(sp)
    .endr
    addi sp, sp, 256

    sret # uzima sepc registar i upisuje u pc
