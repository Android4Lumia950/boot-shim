    AREA s_ArmDeInitialize, CODE, READONLY, ARM
    EXPORT ArmDeInitialize
    EXPORT ArmCleanInvalidateCacheRange
    EXPORT ArmCallSmcHardCoded
    EXPORT ArmReadCntFrq

CTRL_M_BIT      EQU     (1 << 0)
CTRL_C_BIT      EQU     (1 << 2)
CTRL_B_BIT      EQU     (1 << 7)
CTRL_I_BIT      EQU     (1 << 12)
CACHE_LINE      EQU     64

ArmDeInitialize
    ; Disable Branch Prediction
    mrc     p15, 0, r0, c1, c0, 0
    bic     r0, r0, #0x0800
    mcr     p15, 0, r0, c1, c0, 0
    dsb
    isb

    ; Enter critical section: disable interrupt
    cpsid   if
    isb

    ; Clean, invalidate and disable data-cache
    dsb
    mrc     p15, 1, r6, c0, c0, 1        ; Read CLIDR
    lsr     r3, r6, #23
    tst     r3, #7                       ; Check if Level of Coherency (LoC) exists
    beq     Finished
    mov     r10, #0

Loop1
    add     r2, r10, r10, lsr #1         ; Work out 3xcachelevel
    lsr     r12, r6, r2
    and     r12, r12, #7                 ; Extract cache type for this level
    cmp     r12, #2
    blt     Skip                         ; Skip if no cache or only instruction cache at this level
    mcr     p15, 2, r10, c0, c0, 0       ; Write CSSELR (Cache Size selection register)
    isb
    mrc     p15, 1, r12, c0, c0, 0       ; Read CCSIDR (Cache Size ID register)
    and     r2, r12, #7                  ; Extract line length field
    add     r2, r2, #4                   ; Add 4 for line length offset (log2 16 bytes)
    mov     r4, r12, lsr #3
    and     r4, r4, #0x3FF               ; Max way size
    clz     r5, r4                       ; Get bit position of the way size increment
    mov     r7, r12, lsr #13
    and     r7, r7, #0x7FFF              ; Max index size

Loop2
    mov     r9, r4                       ; Working copy of the max way size

Loop3
    orr     r0, r10, r9, lsl r5          ; Factor in way and cache numbers
    orr     r0, r0, r7, lsl r2           ; Factor in index number
    mcr     p15, 0, r0, c7, c14, 2       ; Clean and Invalidate this line
    subs    r9, r9, #1                   ; Decrement the way number
    bge     Loop3
    subs    r7, r7, #1                   ; Decrement the index
    bge     Loop2

Skip
    add     r10, r10, #2                 ; Increment cache level
    cmp     r3, r10
    bgt     Loop1

Finished
    dsb

    ; Invalidate I-Cache
    mcr     p15, 0, r0, c7, c5, 0        ; Invalidate entire instruction cache
    dsb
    isb

    ; Turn off MMU, I-Cache, D-Cache
    mrc     p15, 0, r0, c1, c0, 0        ; Get control register
    bic     r0, r0, #(CTRL_C_BIT | CTRL_I_BIT | CTRL_M_BIT)
    mcr     p15, 0, r0, c1, c0, 0        ; Write control register
    dsb
    isb

    ; Flush TLB
    mov     r0, #0
    mcr     p15, 0, r0, c8, c7, 0        ; Invalidate entire unified TLB
    mcr     p15, 0, r0, c7, c5, 6        ; BPIALL - Invalidate Branch predictor array
    dsb
    isb

    ; Return
    bx      lr

ArmCleanInvalidateCacheRange
    dsb
    add     r2, r0, r1                   ; Calculate the end address
    bic     r0, r0, #(CACHE_LINE-1)      ; Align start with cache line

ArmCleanInvalidateCacheRange0
    mcr     p15, 0, r0, c7, c14, 1       ; Clean & invalidate cache by MVA
    add     r0, r0, #CACHE_LINE
    cmp     r0, r2
    blo     ArmCleanInvalidateCacheRange0

    mov     r0, #0
    dsb

    bx      lr

ArmCallSmcHardCoded
    ; Load the SMC arguments into the appropriate registers
    ldr     r0, =0x0200010f
    ldr     r1, =0x12
    ldr     r2, =0xA0000000
    ldr     r3, =0x50
    mov     r4, #0
    mov     r5, #0
    mov     r6, #0
    mov     r7, #0

    smc     #0
    bx      lr

ArmReadCntFrq
    mrc     p15, 0, r0, c14, c0, 0        ; Read CNTFRQ
    bx      lr

    END
