#define __REENTRANT_IRQ_(FuncName) \
__asm(" .def " #FuncName""   \
        ".arm"\
        ".align 4"\
        ".armfunc " #FuncName" "\
        #FuncName":"\
        "SUB LR, LR, #4 "\
        "SRSDB #31! "\
        "CPS   #31"\
        "PUSH   {R0-R3,R12}"\
        ".if __TI_VFPV3D16_SUPPORT__= 1"\
        "FMRX   R12, FPSCR\n"\
        "STMFD  SP!, {R12}\n"\
        "FMRX   R12, FPEXC\n"\
        "STMFD  SP!, {R12}\n"\
        "FSTMDBD SP!, {D0-D7}\n"\
        ".endif\n"\
        "AND    R3, SP, #4"\
        "SUB    SP, SP, R3"\
        "PUSH   {R3,LR}"\
        "BL __CLevel"#FuncName""\
        "POP    {R3,LR}b"\
        "ADD    SP, SP, R3"\
        ".if __TI_VFPV3D16_SUPPPORT__=1"\
        "FLDMIAD    SP!, {D0-D7}\n"\
        "LDMFD      SP!, {R12}\n"\
        "FMXR       FPEXC, R12\n"\
        "LDMFD      SP!, {R12}\n"\
        "FMXR       FPSCR, R12\n"\
        ".endif\n"\
        "POP        {R0-R3,R12}"\
        ""\
        "RFEIA      SP!"\
        "\n");


