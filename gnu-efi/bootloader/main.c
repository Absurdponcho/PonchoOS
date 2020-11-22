#include <efi.h>
#include <efilib.h>

EFI_STATUS efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
	EFI_STATUS Status;
	EFI_INPUT_KEY Key;

	ST = SystemTable; //Store the system table

	Status = ST->ConOut->OutputString(ST->ConOut, L"HelloWorld!\n\r"); // Print HelloWorld to the console

	if (EFI_ERROR(Status)) //If printing failed, return
		return Status;

	Status = ST->ConIn->Reset(ST->ConIn, FALSE); // Empty the Console Input Buffer
	if (EFI_ERROR(Status))
		return Status;

	while ((Status = ST->ConIn->ReadKeyStroke(ST->ConIn, &Key)) == EFI_NOT_READY) ; // Poll For Key Input

	return Status; // Exit the UEFI application
}
