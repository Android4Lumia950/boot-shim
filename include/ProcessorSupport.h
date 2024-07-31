#pragma once

// Define CACHE_LINE size for cache line alignment
#define CACHE_LINE 64

// Function declarations for ARM processor support
UINTN EFIAPI ArmReadCntFrq(VOID);

VOID EFIAPI ArmDeInitialize(VOID);
VOID EFIAPI ArmCleanInvalidateCacheRange(addr_t start, size_t len);
