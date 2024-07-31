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
    mrc     p15, 1, r6, c0, c0, 1          ; Read CLIDR
    ands    r3, r6, #&7000000              ; Mask out all but Level of Coherency (LoC)
    mov     r3, r3, LSR #23                ; Cache level value (naturally aligned)
    beq     Finished
    mov     r10, #0

Loop1
    add     r2, r10, r10, LSR #1           ; Work out 3xcachelevel
    mov     r12, r6, LSR r2                ; bottom 3 bits are the Cache type for this level
    and     r12, r12, #7                   ; get those 3 bits alone
    cmp     r12, #2
    blt     Skip                           ; no cache or only instruction cache at this level
    mcr     p15, 2, r10, c0, c0, 0         ; write the Cache Size selection register (CSSELR)
    isb                                     ; isb to sync the change to the CacheSizeID reg
    mrc     p15, 1, r12, c0, c0, 0         ; reads current Cache Size ID register (CCSIDR)
    and     r2, r12, #&7                   ; extract the line length field
    add     r2, r2, #4                     ; add 4 for the line length offset (log2 16 bytes)
    ldr     r4, =0x3FF
    ands    r4, r4, r12, LSR #3            ; r4 is the max number on the way size (right aligned)
    clz     r5, r4                         ; r5 is the bit position of the way size increment
    ldr     r7, =0x00007FFF
    ands    r7, r7, r12, LSR #13           ; r7 is the max number of the index size (right aligned)

Loop2
    mov     r9, r4                         ; r9 working copy of the max way size (right aligned)

Loop3
    orr     r0, r10, r9, LSL r5            ; factor in the way number and cache number into r11
    orr     r0, r0, r7, LSL r2             ; factor in the index number

    ; ArmCleanInvalidateDataCacheEntryBySetWay
    mcr     p15, 0, r0, c7, c14, 2         ; Clean and Invalidate this line

    subs    r9, r9, #1                     ; decrement the way number
    bge     Loop3
    subs    r7, r7, #1                     ; decrement the index
    bge     Loop2

Skip
    add     r10, r10, #2                   ; increment the cache number
    cmp     r3, r10
    bgt     Loop1

Finished
    dsb

    ; Invalidate I-Cache
    mcr     p15, 0, r0, c7, c5, 0          ; Invalidate entire instruction cache
    dsb
    isb

    ; Turn off MMU, I-Cache, D-Cache
    mov     r1, #0x0005                    ; Load lower part
    bic     r0, r0, r1                     ; Clear lower bits

    mov     r1, #0x1000                    ; Load upper part
    bic     r0, r0, r1                     ; Clear upper bits

    mcr     p15, 0, r0, c1, c0, 0          ; Write control register
    dsb
    isb

    ; Flush TLB
    mov     r0, #0
    mcr     p15, 0, r0, c8, c7, 0
    mcr     p15, 0, r9, c7, c5, 6          ; BPIALL Invalidate Branch predictor array. R9 == NoOp
    dsb
    isb

    ; Return
    bx      lr

ArmCleanInvalidateCacheRange
    ; cache-ops.S @ lk
    dsb
    add     r2, r0, r1                     ; Calculate the end address
    bic     r0, #(CACHE_LINE-1)            ; Align start with cache line
ArmCleanInvalidateCacheRange0
    mcr     p15, 0, r0, c7, c14, 1         ; Clean & invalidate cache to PoC by MVA
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
    mrc     p15, 0, r0, c14, c0, 0    ; Read CNTFRQ
    bx      lr

    END
