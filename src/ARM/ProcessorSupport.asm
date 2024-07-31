CTRL_M_BIT      EQU     (1 << 0)
CTRL_C_BIT      EQU     (1 << 2)
CTRL_B_BIT      EQU     (1 << 7)
CTRL_I_BIT      EQU     (1 << 12)
CACHE_LINE      EQU     64

    AREA s_ArmDeInitialize, CODE, READONLY, ARM
    EXPORT ArmDeInitialize
    EXPORT ArmCleanInvalidateCacheRange
    EXPORT ArmCallSmcHardCoded
    EXPORT ArmReadCntFrq

ArmDeInitialize
    ; Disable Branch Prediction
    mrc     p15, 0, r0, c1, c0, 0
    bic     r0, r0, #0x00000800
    mcr     p15, 0, r0, c1, c0, 0
    dsb
    isb

    ; Enter critical section: disable interrupt
    cpsid   if
    isb

    ; Clean, invalidate, and disable data-cache
    dsb
    mrc     p15, 1, r6, c0, c0, 1           ; Read CLIDR
    ubfx    r3, r6, #23, #3                 ; Extract Level of Coherency (LoC)
    beq     Finished
    mov     r10, #0

Loop1
    add     r2, r10, r10, lsr #1            ; Calculate 3x cache level
    lsr     r12, r6, r2                     ; Get cache type for this level
    ands    r12, r12, #7                    ; Isolate the cache type
    blt     Skip                            ; No cache or only instruction cache at this level

    mcr     p15, 2, r10, c0, c0, 0          ; Write CSSELR to select cache level
    isb                                     ; Synchronize
    mrc     p15, 1, r12, c0, c0, 0          ; Read CCSIDR
    ubfx    r2, r12, #0, #3                 ; Extract line length
    add     r2, r2, #4                      ; Adjust for log2(16 bytes)
    ubfx    r4, r12, #3, #10                ; Get the number of ways
    clz     r5, r4                          ; Get position of the highest way bit
    ubfx    r7, r12, #13, #15               ; Get the number of sets

Loop2
    mov     r9, r4                          ; Initialize way counter

Loop3
    orr     r0, r10, r9, lsl r5             ; Calculate set/way address
    orr     r0, r0, r7, lsl r2              ; Factor in the index

    ; Clean and Invalidate Data Cache Entry by Set/Way
    mcr     p15, 0, r0, c7, c14, 2

    subs    r9, r9, #1                      ; Decrement way counter
    bge     Loop3
    subs    r7, r7, #1                      ; Decrement set counter
    bge     Loop2

Skip
    add     r10, r10, #2                    ; Increment cache level
    cmp     r3, r10
    bgt     Loop1

Finished
    dsb

    ; Invalidate I-Cache
    mcr     p15, 0, r0, c7, c5, 0           ; Invalidate entire instruction cache
    dsb
    isb

    ; Turn off MMU, I-Cache, D-Cache
    mrc     p15, 0, r0, c1, c0, 0           ; Get control register
    bic     r0, r0, #CTRL_C_BIT             ; Disable D Cache
    bic     r0, r0, #CTRL_I_BIT             ; Disable I Cache
    bic     r0, r0, #CTRL_M_BIT             ; Disable MMU
    mcr     p15, 0, r0, c1, c0, 0           ; Write control register
    dsb
    isb

    ; Flush TLB
    mov     r0, #0
    mcr     p15, 0, r0, c8, c7, 0
    mcr     p15, 0, r0, c7, c5, 6           ; BPIALL Invalidate Branch predictor array
    dsb
    isb

    ; Return
    bx      lr

ArmCleanInvalidateCacheRange
    ; Clean and Invalidate Cache Range
    dsb
    add     r2, r0, r1                      ; Calculate the end address
    bic     r0, r0, #(CACHE_LINE-1)         ; Align start with cache line
ArmCleanInvalidateCacheRange0
    mcr     p15, 0, r0, c7, c14, 1          ; Clean & invalidate cache to PoC by MVA
    add     r0, r0, #CACHE_LINE
    cmp     r0, r2
    blo     ArmCleanInvalidateCacheRange0

    mov     r0, #0
    dsb
    bx      lr

ArmCallSmcHardCoded
    ; Load the SMC arguments values into the appropriate registers
    ldr     r7, =0
    ldr     r6, =0
    ldr     r5, =0
    ldr     r4, =0
    ldr     r3, =0x50
    ldr     r2, =0xA0000000
    ldr     r1, =0x12
    ldr     r0, =0x0200010f

    smc     #0
    bx      lr

ArmReadCntFrq
    ; Read CNTFRQ
    mrc     p15, 0, r0, c14, c0, 0
    bx      lr

    END
