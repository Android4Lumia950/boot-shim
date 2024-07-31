#pragma once

#include <efi.h>
#include <efilib.h>

/* DEFINES *******************************************************************/

// Flags
#define BL_APPLICATION_FLAG_CONVERTED_FROM_EFI          0x01

// Signatures
#define BL_APP_ENTRY_SIGNATURE                          "BTAPENT"
#define BOOT_APPLICATION_SIGNATURE_1                    'TOOB'
#define BOOT_APPLICATION_SIGNATURE_2                    ' PPA'

// Memory Translation Types
#define BOOT_MEMORY_TRANSLATION_TYPE_PHYSICAL           0
#define BOOT_MEMORY_TRANSLATION_TYPE_VIRTUAL            1

// Versions
#define BOOT_APPLICATION_VERSION                        2
#define BL_MEMORY_DATA_VERSION                          1
#define BL_RETURN_ARGUMENTS_VERSION                     1
#define BL_FIRMWARE_DESCRIPTOR_VERSION                  2

// BL Return Arguments Flags
#define BL_RETURN_ARGUMENTS_NO_PAE_FLAG                 0x40

// Application Entry Flags
#define BL_APPLICATION_ENTRY_FLAG_NO_GUID               0x01
#define BL_APPLICATION_ENTRY_BCD_OPTIONS_INTERNAL       0x02
#define BL_APPLICATION_ENTRY_WINLOAD                    0x04
#define BL_APPLICATION_ENTRY_STARTUP                    0x08
#define BL_APPLICATION_ENTRY_REBOOT_ON_ERROR            0x20
#define BL_APPLICATION_ENTRY_NTLDR                      0x40
#define BL_APPLICATION_ENTRY_BCD_OPTIONS_EXTERNAL       0x80
#define BL_APPLICATION_ENTRY_WINRESUME                  0x100
#define BL_APPLICATION_ENTRY_SETUPLDR                   0x200
#define BL_APPLICATION_ENTRY_BOOTSECTOR                 0x400
#define BL_APPLICATION_ENTRY_BOOTMGR                    0x1000
#define BL_APPLICATION_ENTRY_DISPLAY_ORDER              0x800000
#define BL_APPLICATION_ENTRY_FIXED_SEQUENCE             0x20000000
#define BL_APPLICATION_ENTRY_RECOVERY                   0x40000000

// Context Flags
#define BL_CONTEXT_PAGING_ON                            1
#define BL_CONTEXT_INTERRUPTS_ON                        2

// Memory Manager Flags
#define BL_MM_FLAG_USE_FIRMWARE_FOR_MEMORY_MAP_BUFFERS  0x01
#define BL_MM_FLAG_REQUEST_COALESCING                   0x02

// Memory Manager Add Descriptor Flags
#define BL_MM_ADD_DESCRIPTOR_COALESCE_FLAG              0x01
#define BL_MM_ADD_DESCRIPTOR_TRUNCATE_FLAG              0x02
#define BL_MM_ADD_DESCRIPTOR_NEVER_COALESCE_FLAG        0x10
#define BL_MM_ADD_DESCRIPTOR_NEVER_TRUNCATE_FLAG        0x20
#define BL_MM_ADD_DESCRIPTOR_ALLOCATE_FLAG              0x1000
#define BL_MM_ADD_DESCRIPTOR_UPDATE_LIST_POINTER_FLAG   0x2000

// Memory Manager Inclusion Flags
#define BL_MM_INCLUDE_MAPPED_ALLOCATED                  0x01
#define BL_MM_INCLUDE_MAPPED_UNALLOCATED                0x02
#define BL_MM_INCLUDE_UNMAPPED_ALLOCATED                0x04
#define BL_MM_INCLUDE_UNMAPPED_UNALLOCATED              0x08
#define BL_MM_INCLUDE_RESERVED_ALLOCATED                0x10
#define BL_MM_INCLUDE_BAD_MEMORY                        0x20
#define BL_MM_INCLUDE_FIRMWARE_MEMORY                   0x40
#define BL_MM_INCLUDE_TRUNCATED_MEMORY                  0x80
#define BL_MM_INCLUDE_PERSISTENT_MEMORY                 0x100
#define BL_MM_INCLUDE_FIRMWARE_MEMORY_2                 0x200

// Memory Manager Inclusion Masks
#define BL_MM_INCLUDE_NO_FIRMWARE_MEMORY                (BL_MM_INCLUDE_PERSISTENT_MEMORY | \
                                                         BL_MM_INCLUDE_TRUNCATED_MEMORY | \
                                                         BL_MM_INCLUDE_BAD_MEMORY | \
                                                         BL_MM_INCLUDE_RESERVED_ALLOCATED | \
                                                         BL_MM_INCLUDE_UNMAPPED_UNALLOCATED | \
                                                         BL_MM_INCLUDE_UNMAPPED_ALLOCATED | \
                                                         BL_MM_INCLUDE_MAPPED_UNALLOCATED | \
                                                         BL_MM_INCLUDE_MAPPED_ALLOCATED)

#define BL_MM_INCLUDE_ONLY_FIRMWARE_MEMORY              (BL_MM_INCLUDE_FIRMWARE_MEMORY_2 | \
                                                         BL_MM_INCLUDE_FIRMWARE_MEMORY)

// Memory Manager Request Types
#define BL_MM_REQUEST_DEFAULT_TYPE                      1
#define BL_MM_REQUEST_TOP_DOWN_TYPE                     2

// Memory Manager Removal Flags
#define BL_MM_REMOVE_PHYSICAL_REGION_FLAG               0x40000000
#define BL_MM_REMOVE_VIRTUAL_REGION_FLAG                0x80000000

// Library Flags
#define BL_LIBRARY_FLAG_NO_DISPLAY                      0x01
#define BL_LIBRARY_FLAG_REINITIALIZE                    0x02
#define BL_LIBRARY_FLAG_REINITIALIZE_ALL                0x04
#define BL_LIBRARY_FLAG_ZERO_HEAP_ALLOCATIONS_ON_FREE   0x10
#define BL_LIBRARY_FLAG_INITIALIZATION_COMPLETED        0x20
#define BL_LIBRARY_FLAG_NO_GRAPHICS_CONSOLE             0x800

// Display Flags
#define BL_DISPLAY_GRAPHICS_FORCED_VIDEO_MODE_FLAG      0x01
#define BL_DISPLAY_GRAPHICS_FORCED_HIGH_RES_MODE_FLAG   0x02

// HT Flags
#define BL_HT_VALUE_IS_INLINE                           0x01

// File System Flags
#define BL_FS_REGISTER_AT_HEAD_FLAG                     1

// Block Device Flags
#define BL_BLOCK_DEVICE_REMOVABLE_FLAG                  0x01
#define BL_BLOCK_DEVICE_PRESENT_FLAG                    0x02
#define BL_BLOCK_DEVICE_VIRTUAL_FLAG                    0x04

// Memory Class
#define BL_MEMORY_CLASS_SHIFT                           28

// File Access Types
#define BL_FILE_READ_ACCESS                             0x01
#define BL_FILE_WRITE_ACCESS                            0x02
#define BL_DIRECTORY_ACCESS                             0x04
#define BL_UNKNOWN_ACCESS                               0x10

// Device Access Types
#define BL_DEVICE_READ_ACCESS                           0x01
#define BL_DEVICE_WRITE_ACCESS                          0x02

// Device Entry Flags
#define BL_DEVICE_ENTRY_OPENED                          0x01
#define BL_DEVICE_ENTRY_READ_ACCESS                     0x02
#define BL_DEVICE_ENTRY_WRITE_ACCESS                    0x04

// File Entry Flags
#define BL_FILE_ENTRY_OPENED                            0x01
#define BL_FILE_ENTRY_READ_ACCESS                       0x02
#define BL_FILE_ENTRY_WRITE_ACCESS                      0x04
#define BL_FILE_ENTRY_UNKNOWN_ACCESS                    0x10
#define BL_FILE_ENTRY_DIRECTORY                         0x10000

// ETFS File Entry Flags
#define BL_ETFS_FILE_ENTRY_DIRECTORY                    0x01

// Image Types
#define BL_IMG_VALID_FILE                               0x01
#define BL_IMG_MEMORY_FILE                              0x02
#define BL_IMG_REMOTE_FILE                              0x04

// Image Load Flags
#define BL_LOAD_IMG_VIRTUAL_BUFFER                      0x01
#define BL_LOAD_IMG_EXISTING_BUFFER                     0x04
#define BL_LOAD_IMG_UNKNOWN_BUFFER_FLAG                 0x08
#define BL_LOAD_IMG_COMPUTE_SIGNATURE                   0x10
#define BL_LOAD_IMG_COMPUTE_HASH                        0x40000

// PE Image Load Flags
#define BL_LOAD_PE_IMG_VIRTUAL_BUFFER                   BL_LOAD_IMG_VIRTUAL_BUFFER
#define BL_LOAD_PE_IMG_CHECK_MACHINE                    0x02
#define BL_LOAD_PE_IMG_EXISTING_BUFFER                  BL_LOAD_IMG_EXISTING_BUFFER 
#define BL_LOAD_PE_IMG_COMPUTE_HASH                     0x10
#define BL_LOAD_PE_IMG_CHECK_SUBSYSTEM                  0x80
#define BL_LOAD_PE_IMG_SKIP_RELOCATIONS                 0x100
#define BL_LOAD_PE_IMG_CHECK_FORCED_INTEGRITY           0x200
#define BL_LOAD_PE_IMG_IGNORE_CHECKSUM_MISMATCH         0x10000
#define BL_LOAD_PE_IMG_VALIDATE_ORIGINAL_FILENAME       0x400000

// Checksum Utilities
#define BL_UTL_CHECKSUM_COMPLEMENT                      0x10000
#define BL_UTL_CHECKSUM_ROTATE                          0x20000
#define BL_UTL_CHECKSUM_NEGATE                          0x40000
#define BL_UTL_CHECKSUM_UCHAR_BUFFER                    0x01
#define BL_UTL_CHECKSUM_USHORT_BUFFER                   0x02

/* ENUMERATIONS **************************************************************/

// Colors
typedef enum _BL_COLOR
{
    Black,
    Blue,
    Green,
    Cyan,
    Red,
    Magenta,
    Brown,
    LtGray,
    Gray,
    LtBlue,
    LtGreen,
    LtCyan,
    LtRed,
    LtMagenta,
    Yellow,
    White
} BL_COLOR, *PBL_COLOR;

// Menu Policies
typedef enum _BL_MENU_POLICY
{
    MenuPolicyLegacy = 0,
    MenuPolicyStandard = 1
} BL_MENU_POLICY;

// Memory Descriptor Types
typedef enum _BL_MEMORY_DESCRIPTOR_TYPE
{
    BlMdPhysical,
    BlMdVirtual,
    BlMdTracker
} BL_MEMORY_DESCRIPTOR_TYPE;

// Translation Types
typedef enum _BL_TRANSLATION_TYPE
{
    BlNone,
    BlVirtual,
    BlPae,
    BlMax
} BL_TRANSLATION_TYPE;

// Architecture Modes
typedef enum _BL_ARCH_MODE
{
    BlProtectedMode,
    BlRealMode
} BL_ARCH_MODE;

// Boot Device Types
typedef enum _BL_DEVICE_TYPE
{
    DiskDevice = 0,
    LegacyPartitionDevice = 2,
    SerialDevice = 3,
    UdpDevice = 4,
    BootDevice = 5,
    PartitionDevice = 6,
    LocateDevice = 8,
} BL_DEVICE_TYPE;

// Local Device Types
typedef enum _BL_LOCAL_DEVICE_TYPE
{
    LocalDevice = 0,
    FloppyDevice = 1,
    CdRomDevice = 2,
    RamDiskDevice = 3,
    FileDevice = 5,
    VirtualDiskDevice = 6
} BL_LOCAL_DEVICE_TYPE;

// Partition Types
typedef enum _BL_PARTITION_TYPE
{
    GptPartition,
    MbrPartition,
    RawPartition,
} BL_PARTITION_TYPE;

// File Path Types
typedef enum _BL_FILE_PATH_TYPE
{
    BlFilePath = 0,
    BlDirectoryPath = 1,
} BL_FILE_PATH_TYPE;

/* STRUCTURES ****************************************************************/

// Boot Application Entry
typedef struct _BL_APP_ENTRY
{
    EFI_GUID  Guid;
    UINT32    Flags;
    UINT32    Attributes;
    UINT32    LoadOptionsSize;
    VOID      *LoadOptions;
    UINT64    EntryPoint;
} BL_APP_ENTRY, *PBL_APP_ENTRY;

// Boot Firmware Descriptor
typedef struct _BL_FIRMWARE_DESCRIPTOR
{
    EFI_GUID Guid;
    UINT32   Version;
    UINT32   Size;
    VOID     *FirmwareData;
} BL_FIRMWARE_DESCRIPTOR, *PBL_FIRMWARE_DESCRIPTOR;

// Memory Descriptor
typedef struct _BL_MEMORY_DESCRIPTOR
{
    EFI_PHYSICAL_ADDRESS PhysicalStart;
    EFI_PHYSICAL_ADDRESS VirtualStart;
    UINT64                NumberOfPages;
    UINT32                Type;
    UINT32                Reserved;
} BL_MEMORY_DESCRIPTOR, *PBL_MEMORY_DESCRIPTOR;

// Memory Map
typedef struct _BL_MEMORY_MAP
{
    UINT32                DescriptorSize;
    UINT32                DescriptorVersion;
    UINT32                MapSize;
    UINT32                MapVersion;
    BL_MEMORY_DESCRIPTOR  *Map;
} BL_MEMORY_MAP, *PBL_MEMORY_MAP;

// Boot Context
typedef struct _BL_CONTEXT
{
    UINT32                Flags;
    EFI_HANDLE            ImageHandle;
    EFI_SYSTEM_TABLE      *SystemTable;
    EFI_BOOT_SERVICES     *BootServices;
    EFI_RUNTIME_SERVICES  *RuntimeServices;
} BL_CONTEXT, *PBL_CONTEXT;

// Firmware Descriptor
typedef struct _BL_FIRMWARE_DESCRIPTOR
{
    EFI_GUID  Guid;
    UINT32    Version;
    UINT32    Size;
    VOID      *FirmwareData;
} BL_FIRMWARE_DESCRIPTOR, *PBL_FIRMWARE_DESCRIPTOR;

// File Handle
typedef struct _BL_FILE_HANDLE
{
    EFI_FILE_PROTOCOL *FileProtocol;
    EFI_HANDLE        FileHandle;
    UINT32            FileAccess;
} BL_FILE_HANDLE, *PBL_FILE_HANDLE;

/* FUNCTION PROTOTYPES *******************************************************/

// Function declarations go here

