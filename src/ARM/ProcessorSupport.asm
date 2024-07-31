        AREA s_ArmDeInitialize, CODE, READONLY, ARM
        EXPORT ArmDeInitialize
        EXPORT ArmCleanInvalidateCacheRange
        EXPORT ArmCallSmcHardCoded
        EXPORT ArmReadCntFrq

        ; Define control register bit masks
CTRL_M_BIT      EQU     1 << 0
CTRL_C_BIT      EQU     1 << 2
CTRL_I_BIT      EQU     1 << 12

        ; Define combined control register bits
CTRL_COMBINED
        EQU     CTRL_C_BIT | CTRL_I_BIT | CTRL_M_BIT

        ; Define cache line size
CACHE_LINE      EQU     64

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

    ; Clean, invalidate and disable data-cache
    dsb
    mrc     p15, 1, r6, c0, c0, 1       ; Read CLIDR
    ands    r3, r6, #0x7000000          ; Mask out all but Cache Level ID
    mov     r3, r3, LSR #23             ; Cache level value (naturally aligned)
    beq     Finished
    mov     r10, #0

Loop1
    add     r2, r10, r10, LSR #1        ; Compute cache level multiplier
    mov     r12, r6, LSR r2             ; Read Cache Size ID register
    and     r12, r12, #7                ; Get cache type
    cmp     r12, #2
    blt     Skip                        ; Skip if no cache or only instruction cache

    mcr     p15, 2, r10, c0, c0, 0     ; Select cache level
    isb                                 ; Synchronize cache size
    mrc     p15, 1, r12, c0, c0, 0     ; Read Cache Size ID register
    and     r2, r12, #7                ; Extract line length
    add     r2, r2, #4                 ; Add offset for line length
    ldr     r4, =0x3FF
    ands    r4, r4, r12, LSR #3        ; Maximum number of ways
    clz     r5, r4                     ; Bit position for way size
    ldr     r7, =0x00007FFF
    ands    r7, r7, r12, LSR #13       ; Maximum number of index sizes

Loop2
    mov     r9, r4                     ; Way size

Loop3
    orr     r0, r10, r9, LSL r5        ; Compute cache line address
    orr     r0, r0, r7, LSL r2        ; Factor in index number

    ; Clean and Invalidate Data Cache Entry
    mcr     p15, 0, r0, c7, c14, 2    ; Clean and invalidate cache line
    subs    r9, r9, #1                ; Decrement way number
    bge     Loop3
    subs    r7, r7, #1                ; Decrement index number
    bge     Loop2

Skip
    add     r10, r10, #2              ; Move to the next cache level
    cmp     r3, r10
    bgt     Loop1

Finished
    dsb

    ; Invalidate I-Cache
    mcr     p15, 0, r0, c7, c5, 0    ; Invalidate instruction cache
    dsb
    isb

    ; Disable MMU, I-Cache, D-Cache
    mrc     p15, 0, r0, c1, c0, 0    ; Read control register
    bic     r0, r0, #CTRL_COMBINED    ; Disable caches and MMU
    mcr     p15, 0, r0, c1, c0, 0    ; Write control register
    dsb
    isb

    ; Flush TLB
    mov     r0, #0
    mcr     p15, 0, r0, c8, c7, 0
    mcr     p15, 0, r0, c7, c5, 6    ; Invalidate Branch Predictor Array
    dsb
    isb

    ; Return
    bx      lr

ArmCleanInvalidateCacheRange
    ; Clean & Invalidate Cache Range
    dsb
    add     r2, r0, r1                ; End address
    bic     r0, r0, #CACHE_LINE - 1  ; Align start address
ArmCleanInvalidateCacheRange0
    mcr     p15, 0, r0, c7, c14, 1   ; Clean & invalidate cache line
    add     r0, r0, #CACHE_LINE
    cmp     r0, r2
    blo     ArmCleanInvalidateCacheRange0

    mov     r0, #0
    dsb

    bx      lr

ArmCallSmcHardCoded
    ; Load SMC arguments
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
    mrc     p15, 0, r0, c14, c0, 0   ; Read CNTFRQ
    bx      lr

        END
