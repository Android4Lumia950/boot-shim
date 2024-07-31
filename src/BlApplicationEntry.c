#include <efi.h>
#include <efilib.h>

#include "application.h"

// Boot Manager Application Entry Point
// Initializes environment and transfers control to the EFI application entry point.
NTSTATUS BlApplicationEntry(
    _In_ PBOOT_APPLICATION_PARAMETER_BLOCK BootAppParameters,
    _In_ PBL_LIBRARY_PARAMETERS LibraryParameters
)
{
    // Check for null BootAppParameters
    if (!BootAppParameters)
    {
        return STATUS_INVALID_PARAMETER;
    }

    // Initialize local variables
    PBL_FIRMWARE_DESCRIPTOR FirmwareDescriptor;
    UINTN ParamPointer = (UINTN)BootAppParameters;
    FirmwareDescriptor = (PBL_FIRMWARE_DESCRIPTOR)(ParamPointer + BootAppParameters->FirmwareParametersOffset);

    // Switch to real mode context
    SwitchToRealModeContext(FirmwareDescriptor);

    // Call EFI main if SystemTable is present
    if (FirmwareDescriptor->SystemTable)
    {
        efi_main(
            FirmwareDescriptor->ImageHandle, 
            FirmwareDescriptor->SystemTable
        );
    }

    // Return success
    return STATUS_SUCCESS;
}
