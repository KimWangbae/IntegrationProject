;-------------------------------------------------------------------------------
; HL_sys_mpu.asm
;
; (c) Texas Instruments 2009-2013, All rights reserved.
;

    .text
    .arm


;-------------------------------------------------------------------------------
; Initalize Mpu

    .def     _mpuInit_
    .asmfunc

_mpuInit_
        stmfd sp!, {r0}
        ; Disable mpu
        mrc   p15, #0, r0, c1, c0, #0
        bic   r0,  r0, #1
        dsb
        mcr   p15, #0, r0, c1, c0, #0
        isb
        ; Disable background region
        mrc   p15, #0, r0,      c1, c0, #0
        bic   r0,  r0, #0x20000
        mcr   p15, #0, r0,      c1, c0, #0
        ; Setup region 1
        mov   r0,  #0
        mcr   p15, #0,    r0, c6, c2, #0
        ldr   r0,  r1Base
        mcr   p15, #0,    r0, c6, c1, #0
        mov   r0,  #0x0000
        orr   r0,  r0,    #0x1200
        mcr   p15, #0,    r0, c6, c1, #4
        movw  r0,  #((1 << 15) + (0 << 14) + (0 << 13) + (0 << 12) + (0 << 11) + (0 << 10) + (1 <<  9) + (0 <<  8) + (0x1D << 1) + (1)) 
        mcr   p15, #0,    r0, c6, c1, #2
        ; Setup region 2
        mov   r0,  #1
        mcr   p15, #0,    r0, c6, c2, #0
        ldr   r0,  r2Base
        mcr   p15, #0,    r0, c6, c1, #0
        mov   r0,  #0x0002
        orr   r0,  r0,    #0x0100
        mcr   p15, #0,    r0, c6, c1, #4
        movw  r0,  #((0 << 15) + (0 << 14) + (0 << 13) + (0 << 12) + (0 << 11) + (0 << 10) + (0 <<  9) + (0 <<  8) + (0x16 << 1) + (1))
        mcr   p15, #0,    r0, c6, c1, #2
        ; Setup region  
        mov   r0,  #2
        mcr   p15, #0,    r0, c6, c2, #0
        ldr   r0,  r3Base
        mcr   p15, #0,    r0, c6, c1, #0    
        mov   r0,  #0x0002
        orr   r0,  r0,    #0x0300
        mcr   p15, #0,    r0, c6, c1, #4
        movw  r0,  #((0 << 15) + (0 << 14) + (0 << 13) + (0 << 12) + (0 << 11) + (0 << 10) + (0 <<  9) + (0 <<  8) + (0x16 << 1) + (1))
        mcr   p15, #0,    r0, c6, c1, #2
        ; Setup region 4
        mov   r0,  #3
        mcr   p15, #0,    r0, c6, c2, #0
        ldr   r0,  r4Base
        mcr   p15, #0,    r0, c6, c1, #0
        mov   r0,  #0x0010
        orr   r0,  r0,    #0x0100
        mcr   p15, #0,    r0, c6, c1, #4
        movw  r0,  #((1 << 15) + (1 << 14) + (0 << 13) + (0 << 12) + (0 << 11) + (0 << 10) + (0 <<  9) + (0 <<  8) + (0x1B << 1) + (1))
        mcr   p15, #0,    r0, c6, c1, #2
        ; Setup region 5
        mov   r0,  #4
        mcr   p15, #0,    r0, c6, c2, #0
        ldr   r0,  r5Base
        mcr   p15, #0,    r0, c6, c1, #0
        mov   r0,  #0x0010
        orr   r0,  r0,    #0x0300
        mcr   p15, #0,    r0, c6, c1, #4
        movw  r0,  #((1 << 15) + (1 << 14) + (0 << 13) + (0 << 12) + (0 << 11) + (0 << 10) + (0 <<  9) + (0 <<  8) + (0x1B << 1) + (1))
        mcr   p15, #0,    r0, c6, c1, #2
        ; Setup region 6
        mov   r0,  #5
        mcr   p15, #0,    r0, c6, c2, #0
        ldr   r0,  r6Base
        mcr   p15, #0,    r0, c6, c1, #0
        mov   r0,  #0x0002
        orr   r0,  r0,    #0x0100
        mcr   p15, #0,    r0, c6, c1, #4
        movw  r0,  #((0 << 15) + (0 << 14) + (0 << 13) + (0 << 12) + (0 << 11) + (0 << 10) + (0 <<  9) + (0 <<  8) + (0x12 << 1) + (1))
        mcr   p15, #0,    r0, c6, c1, #2
        ; Setup region 7
        mov   r0,  #6
        mcr   p15, #0,    r0, c6, c2, #0
        ldr   r0,  r7Base
        mcr   p15, #0,    r0, c6, c1, #0
        mov   r0,  #0x0002
        orr   r0,  r0,    #0x1300
        mcr   p15, #0,    r0, c6, c1, #4
        movw  r0,  #((0 << 15) + (0 << 14) + (0 << 13) + (0 << 12) + (0 << 11) + (0 << 10) + (0 <<  9) + (0 <<  8) + (0x12 << 1) + (1))
        mcr   p15, #0,    r0, c6, c1, #2
        ; Setup region 8
        mov   r0,  #7
        mcr   p15, #0,    r0, c6, c2, #0
        ldr   r0,  r8Base
        mcr   p15, #0,    r0, c6, c1, #0
        mov   r0,  #0x0002
        orr   r0,  r0,    #0x1300
        mcr   p15, #0,    r0, c6, c1, #4
        movw  r0,  #((0 << 15) + (0 << 14) + (0 << 13) + (0 << 12) + (0 << 11) + (0 << 10) + (0 <<  9) + (0 <<  8) + (0x12 << 1) + (1))
        mcr   p15, #0,    r0, c6, c1, #2
        ; Setup region 9
        mov   r0,  #8
        mcr   p15, #0,    r0, c6, c2, #0
        ldr   r0,  r9Base
        mcr   p15, #0,    r0, c6, c1, #0
        mov   r0,  #0x000C
        orr   r0,  r0,    #0x1200
        mcr   p15, #0,    r0, c6, c1, #4
        movw  r0,  #((0 << 15) + (1 << 14) + (1 << 13) + (1 << 12) + (1 << 11) + (1 << 10) + (1 <<  9) + (1 <<  8) + (0x0E << 1) + (1))
        mcr   p15, #0,    r0, c6, c1, #2
        ; Setup region 10
        mov   r0,  #9
        mcr   p15, #0,    r0, c6, c2, #0
        ldr   r0,  r10Base
        mcr   p15, #0,    r0, c6, c1, #0
        mov   r0,  #0x000C
        orr   r0,  r0,    #0x1300
        mcr   p15, #0,    r0, c6, c1, #4
        movw  r0,  #((1 << 15) + (0 << 14) + (1 << 13) + (1 << 12) + (1 << 11) + (1 << 10) + (1 <<  9) + (1 <<  8) + (0x0F << 1) + (1))
        mcr   p15, #0,    r0, c6, c1, #2
        ; Setup region 11
        mov   r0,  #10
        mcr   p15, #0,    r0, c6, c2, #0
        ldr   r0,  r11Base
        mcr   p15, #0,    r0, c6, c1, #0
        mov   r0,  #0x0008
        orr   r0,  r0,    #0x0600
        mcr   p15, #0,    r0, c6, c1, #4
        movw  r0,  #((0 << 15) + (0 << 14) + (0 << 13) + (0 << 12) + (0 << 11) + (0 << 10) + (0 <<  9) + (0 <<  8) + (0x15 << 1) + (1))
        mcr   p15, #0,    r0, c6, c1, #2
        ; Setup region 12
        mov   r0,  #11
        mcr   p15, #0,    r0, c6, c2, #0
        ldr   r0,  r12Base
        mcr   p15, #0,    r0, c6, c1, #0
        mov   r0,  #0x0010
        orr   r0,  r0,    #0x1600
        mcr   p15, #0,    r0, c6, c1, #4
        movw  r0,  #((1 << 15) + (1 << 14) + (1 << 13) + (1 << 12) + (1 << 11) + (1 << 10) + (1 <<  9) + (1 <<  8) + (0x15 << 1) + (1))
        mcr   p15, #0,    r0, c6, c1, #2
        ; Setup region 13
        mov   r0,  #12
        mcr   p15, #0,    r0, c6, c2, #0
        ldr   r0,  r13Base
        mcr   p15, #0,    r0, c6, c1, #0
        mov   r0,  #0x0010
        orr   r0,  r0,    #0x1600
        mcr   p15, #0,    r0, c6, c1, #4
        movw  r0,  #((1 << 15) + (1 << 14) + (0 << 13) + (0 << 12) + (0 << 11) + (0 << 10) + (0 <<  9) + (0 <<  8) + (0x16 << 1) + (1))
        mcr   p15, #0,    r0, c6, c1, #2
        ; Setup region 14
        mov   r0,  #13
        mcr   p15, #0,    r0, c6, c2, #0
        ldr   r0,  r14Base
        mcr   p15, #0,    r0, c6, c1, #0
        mov   r0,  #0x0010
        orr   r0,  r0,    #0x1100
        mcr   p15, #0,    r0, c6, c1, #4
        movw  r0,  #((0 << 15) + (0 << 14) + (1 << 13) + (0 << 12) + (0 << 11) + (0 << 10) + (1 <<  9) + (1 <<  8) + (0x1A << 1) + (1))
        mcr   p15, #0,    r0, c6, c1, #2
        ; Setup region 15
        mov   r0,  #14
        mcr   p15, #0,    r0, c6, c2, #0
        ldr   r0,  r15Base
        mcr   p15, #0,    r0, c6, c1, #0
        mov   r0,  #0x0010
        orr   r0,  r0,    #0x1300
        mcr   p15, #0,    r0, c6, c1, #4
        movw  r0,  #((0 << 15) + (0 << 14) + (1 << 13) + (0 << 12) + (0 << 11) + (0 << 10) + (1 <<  9) + (1 <<  8) + (0x1A << 1) + (1))
        mcr   p15, #0,    r0, c6, c1, #2
        ; Setup region 16
        mov   r0,  #15
        mcr   p15, #0,    r0, c6, c2, #0
        ldr   r0,  r16Base
        mcr   p15, #0,    r0, c6, c1, #0
        mov   r0,  #0x0000
        orr   r0,  r0,    #0x1300
        mcr   p15, #0,    r0, c6, c1, #4
        movw  r0,  #((0 << 15) + (0 << 14) + (0 << 13) + (0 << 12) + (0 << 11) + (0 << 10) + (0 <<  9) + (0 <<  8) + (0x1B << 1) + (0))
        mcr   p15, #0,    r0, c6, c1, #2

        ; Enable mpu
        mrc   p15, #0, r0, c1, c0, #0
        orr   r0,  r0, #1
        dsb
        mcr   p15, #0, r0, c1, c0, #0
        isb
        ldmfd sp!, {r0}
        bx    lr

r1Base  .word 0x00000000  
r2Base  .word 0x80000000  
r3Base  .word 0x80000000  
r4Base  .word 0x60000000  
r5Base  .word 0x60000000  
r6Base  .word 0x08000000  
r7Base  .word 0x08000000  
r8Base  .word 0x08000000  
r9Base  .word 0x08070000  
r10Base  .word 0x08070000  
r11Base  .word 0x00000000  
r12Base  .word 0x00000000  
r13Base  .word 0xF0000000  
r14Base  .word 0xF8000000  
r15Base  .word 0xF8000000  
r16Base  .word 0xF0000000  
    .endasmfunc


;-------------------------------------------------------------------------------
; Enable Mpu

    .def     _mpuEnable_
    .asmfunc

_mpuEnable_

        stmfd sp!, {r0}
        mrc   p15, #0, r0, c1, c0, #0
        orr   r0,  r0, #1
        dsb
        mcr   p15, #0, r0, c1, c0, #0
        isb
        ldmfd sp!, {r0}		
        bx    lr

    .endasmfunc


;-------------------------------------------------------------------------------
; Disable Mpu

    .def     _mpuDisable_
    .asmfunc

_mpuDisable_

        stmfd sp!, {r0}
        mrc   p15, #0, r0, c1, c0, #0
        bic   r0,  r0, #1
        dsb
        mcr   p15, #0, r0, c1, c0, #0
        isb
        ldmfd sp!, {r0}		
        bx    lr

    .endasmfunc


;-------------------------------------------------------------------------------
; Enable Mpu background region

    .def     _mpuEnableBackgroundRegion_
    .asmfunc

_mpuEnableBackgroundRegion_

        stmfd sp!, {r0}
        mrc   p15, #0, r0,      c1, c0, #0
        orr   r0,  r0, #0x20000
        mcr   p15, #0, r0,      c1, c0, #0
        ldmfd sp!, {r0}		
        bx    lr

    .endasmfunc


;-------------------------------------------------------------------------------
; Disable Mpu background region

    .def     _mpuDisableBackgroundRegion_
    .asmfunc

_mpuDisableBackgroundRegion_

        stmfd sp!, {r0}
        mrc   p15, #0, r0,      c1, c0, #0
        bic   r0,  r0, #0x20000
        mcr   p15, #0, r0,      c1, c0, #0
        ldmfd sp!, {r0}
        bx    lr

    .endasmfunc


;-------------------------------------------------------------------------------
; Returns number of implemented Mpu regions

    .def     _mpuGetNumberOfRegions_
    .asmfunc

_mpuGetNumberOfRegions_

        mrc   p15, #0, r0,      c0, c0, #4
        uxtb  r0,  r0, ROR #8
        bx    lr

    .endasmfunc


;-------------------------------------------------------------------------------
; Returns the type of the implemented mpu regions

    .def     _mpuAreRegionsSeparate_
    .asmfunc

_mpuAreRegionsSeparate_

        mrc   p15, #0, r0,      c0, c0, #4
        uxtb  r0,  r0
        bx    lr

    .endasmfunc


;-------------------------------------------------------------------------------
; Set mpu region number

    .def     _mpuSetRegion_
    .asmfunc

_mpuSetRegion_

        mcr   p15, #0, r0, c6, c2, #0
        bx    lr

    .endasmfunc


;-------------------------------------------------------------------------------
; Get mpu region number

    .def     _mpuGetRegion_
    .asmfunc

_mpuGetRegion_

        mrc   p15, #0, r0, c6, c2, #0
        bx    lr

    .endasmfunc


;-------------------------------------------------------------------------------
; Set base address

    .def     _mpuSetRegionBaseAddress_
    .asmfunc

_mpuSetRegionBaseAddress_

        mcr   p15, #0, r0, c6, c1, #0
        bx    lr

    .endasmfunc


;-------------------------------------------------------------------------------
; Get base address

    .def     _mpuGetRegionBaseAddress_
    .asmfunc

_mpuGetRegionBaseAddress_

        mrc   p15, #0, r0, c6, c1, #0
        bx    lr

    .endasmfunc


;-------------------------------------------------------------------------------
; Set type and permission

    .def     _mpuSetRegionTypeAndPermission_
    .asmfunc

_mpuSetRegionTypeAndPermission_

        orr   r0,  r0, r1
        mcr   p15, #0, r0, c6, c1, #4
        bx    lr

    .endasmfunc


;-------------------------------------------------------------------------------
; Get type

    .def     _mpuGetRegionType_
    .asmfunc

_mpuGetRegionType_

        mrc   p15, #0, r0,     c6, c1, #4
        bic   r0,  r0, #0xFF00
        bx    lr

    .endasmfunc


;-------------------------------------------------------------------------------
; Get permission

    .def     _mpuGetRegionPermission_
    .asmfunc

_mpuGetRegionPermission_

        mrc   p15, #0, r0,   c6, c1, #4
        bic   r0,  r0, #0xFF
        bx    lr

    .endasmfunc


;-------------------------------------------------------------------------------
; Set region size register value

    .def     _mpuSetRegionSizeRegister_
    .asmfunc

_mpuSetRegionSizeRegister_

        mcr   p15, #0, r0, c6, c1, #2
        bx    lr

    .endasmfunc

    

;-------------------------------------------------------------------------------

