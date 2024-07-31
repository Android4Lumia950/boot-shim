#include "EFIApp.h"
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/PrintLib.h>

// Function to display "Android" on the screen
VOID DisplayAndroidLogo(EFI_SYSTEM_TABLE *SystemTable) {
    EFI_STATUS Status;
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut = SystemTable->ConOut;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *GraphicsOutput;
    EFI_INPUT_KEY Key;
    UINTN Index;
    CHAR16 *Message = L"Android";

    // Clear the screen
    Status = ConOut->ClearScreen(ConOut);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to clear the screen.\n");
        return;
    }

    // Set the cursor position
    ConOut->SetCursorPosition(ConOut, 0, 0);

    // Output the message
    ConOut->OutputString(ConOut, Message);

    // Wait for any key press
    Print(L"Press any key to continue...\n");
    SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &Key);
}

// Function to jump to a given address
VOID JumpToAddress(
    EFI_HANDLE ImageHandle, 
    EFI_PHYSICAL_ADDRESS Address,
    VOID* PayloadBuffer,
    UINTN PayloadLength
) {
    EFI_STATUS Status;
    UINTN MemMapSize = 0;
    EFI_MEMORY_DESCRIPTOR* MemMap = NULL;
    UINTN MapKey = 0;
    UINTN DesSize = 0;
    UINT32 DesVersion = 0;

    VOID (*entry)() = (VOID (*)()) Address;

    Status = gBS->GetMemoryMap(&MemMapSize, MemMap, &MapKey, &DesSize, &DesVersion);
    if (Status == EFI_BUFFER_TOO_SMALL) {
        MemMapSize += sizeof(EFI_MEMORY_DESCRIPTOR) * 256;
        Status = gBS->AllocatePool(EfiLoaderData, MemMapSize, (VOID**)&MemMap);
        if (EFI_ERROR(Status)) {
            return;
        }
        Status = gBS->GetMemoryMap(&MemMapSize, MemMap, &MapKey, &DesSize, &DesVersion);
    }

    if (EFI_ERROR(Status)) {
        return;
    }

    Status = gBS->ExitBootServices(ImageHandle, MapKey);
    if (EFI_ERROR(Status)) {
        return;
    }

    SetMem((VOID*)Address, PayloadLength, 0xFF);
    CopyMem((VOID*)Address, PayloadBuffer, PayloadLength);

    ArmDeInitialize();
    writel(0, GIC_DIST_CTRL);

    entry();
}

// Function to check ELF32 header validity
BOOLEAN CheckElf32Header(Elf32_Ehdr* bl_elf_hdr) {
    if (!bl_elf_hdr) return FALSE;

    if (bl_elf_hdr->e_ident[EI_MAG0] != ELFMAG0 ||
        bl_elf_hdr->e_ident[EI_MAG1] != ELFMAG1 ||
        bl_elf_hdr->e_ident[EI_MAG2] != ELFMAG2 ||
        bl_elf_hdr->e_ident[EI_MAG3] != ELFMAG3) {
        return FALSE;
    }

    if (bl_elf_hdr->e_machine != EM_ARM || bl_elf_hdr->e_type != ET_EXEC) {
        return FALSE;
    }

    if (bl_elf_hdr->e_phnum < 1) {
        return FALSE;
    }

    return TRUE;
}

// Main entry point of the UEFI application
EFI_STATUS efi_main(
    EFI_HANDLE ImageHandle, 
    EFI_SYSTEM_TABLE *SystemTable
) {
    EFI_STATUS Status = EFI_SUCCESS;
    UINTN NumHandles = 0;
    EFI_HANDLE *SfsHandles = NULL;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *EfiSfsProtocol = NULL;
    EFI_FILE_PROTOCOL *FileProtocol = NULL;
    EFI_FILE_PROTOCOL *PayloadFileProtocol = NULL;
    CHAR16 *PayloadFileName = PAYLOAD_BINARY_NAME;
    EFI_PHYSICAL_ADDRESS UefiEntryPoint = PAYLOAD_ENTRY_POINT_ADDR_INVALID;
    UINTN PayloadFileBufferSize = 0;
    VOID* PayloadFileBuffer = NULL;
    VOID* PayloadLoadSec = NULL;
    EFI_FILE_INFO *PayloadFileInformation = NULL;
    UINTN PayloadFileInformationSize = 0;
    Elf32_Ehdr* PayloadElf32Ehdr = NULL;
    Elf32_Phdr* PayloadElf32Phdr = NULL;
    UINTN PayloadSectionOffset = 0;
    UINTN PayloadLength = 0;

#if defined(_GNU_EFI)
    InitializeLib(ImageHandle, SystemTable);
#endif

    // Display the Android logo or text
    DisplayAndroidLogo(SystemTable);

    Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiSimpleFileSystemProtocolGuid, NULL, &NumHandles, &SfsHandles);
    if (EFI_ERROR(Status)) {
        goto exit;
    }

    for (UINTN index = 0; index < NumHandles; index++) {
        Status = gBS->HandleProtocol(SfsHandles[index], &gEfiSimpleFileSystemProtocolGuid, (VOID**)&EfiSfsProtocol);
        if (EFI_ERROR(Status)) {
            continue;
        }

        Status = EfiSfsProtocol->OpenVolume(EfiSfsProtocol, &FileProtocol);
        if (EFI_ERROR(Status)) {
            continue;
        }

        Status = FileProtocol->Open(FileProtocol, &PayloadFileProtocol, PayloadFileName, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY | EFI_FILE_HIDDEN | EFI_FILE_SYSTEM);
        if (EFI_ERROR(Status)) {
            continue;
        }

        Status = PayloadFileProtocol->GetInfo(PayloadFileProtocol, &gEfiFileInfoGuid, &PayloadFileInformationSize, (VOID*)PayloadFileInformation);
        if (Status == EFI_BUFFER_TOO_SMALL) {
            Status = gBS->AllocatePool(EfiLoaderData, PayloadFileInformationSize, (VOID**)&PayloadFileInformation);
            if (EFI_ERROR(Status)) {
                goto local_cleanup;
            }
            Status = PayloadFileProtocol->GetInfo(PayloadFileProtocol, &gEfiFileInfoGuid, &PayloadFileInformationSize, (VOID*)PayloadFileInformation);
        }

        if (EFI_ERROR(Status)) {
            goto local_cleanup;
        }

        if (PayloadFileInformation->FileSize > UINT32_MAX) {
            goto local_cleanup;
        }

        PayloadFileBufferSize = (UINTN)PayloadFileInformation->FileSize;

        Status = gBS->AllocatePool(EfiLoaderData, PayloadFileBufferSize, &PayloadFileBuffer);
        if (EFI_ERROR(Status)) {
            goto local_cleanup;
        }

        Status = PayloadFileProtocol->Read(PayloadFileProtocol, &PayloadFileBufferSize, PayloadFileBuffer);
        if (EFI_ERROR(Status)) {
            goto local_cleanup;
        }

        PayloadElf32Ehdr = (Elf32_Ehdr*)PayloadFileBuffer;
        if (!CheckElf32Header(PayloadElf32Ehdr)) {
            goto local_cleanup;
        }

        PayloadElf32Phdr = (Elf32_Phdr*)((UINTN)PayloadFileBuffer + PayloadElf32Ehdr->e_phoff);
        UefiEntryPoint = PayloadElf32Ehdr->e_entry;

        for (UINTN ph_idx = 0; ph_idx < PayloadElf32Ehdr->e_phnum; ph_idx++) {
            Elf32_Phdr* Phdr = &PayloadElf32Phdr[ph_idx];
            if (Phdr->p_type != PT_LOAD || Phdr->p_paddr != Phdr->p_vaddr || Phdr->p_filesz != Phdr->p_memsz || Phdr->p_paddr != UefiEntryPoint) {
                continue;
            }

            PayloadSectionOffset = Phdr->p_offset;
            PayloadLength = Phdr->p_memsz;
            break;
        }

        if (PayloadSectionOffset == 0 || PayloadLength == 0) {
            goto local_cleanup;
        }

        PayloadLoadSec = (VOID*)((UINTN)PayloadFileBuffer + PayloadSectionOffset);

        if (UefiEntryPoint > UINT32_MAX) {
            Status = EFI_INVALID_PARAMETER;
            goto local_cleanup;
        }

        JumpToAddress(ImageHandle, UefiEntryPoint, PayloadLoadSec, PayloadLength);

    local_cleanup:
        if (PayloadFileBuffer) gBS->FreePool(PayloadFileBuffer);
        if (PayloadFileInformation) gBS->FreePool(PayloadFileInformation);
        if (PayloadFileProtocol) PayloadFileProtocol->Close(PayloadFileProtocol);
    }

exit:
    if (SfsHandles) gBS->FreePool(SfsHandles);
    if (EFI_ERROR(Status)) gBS->Stall(SECONDS_TO_MICROSECONDS(5));
    return Status;
}
