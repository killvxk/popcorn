//#define EFIAPI __attribute__((ms_abi))

#include <efi.h>
#include <efilib.h>

#define check_status(s, msg)   if(EFI_ERROR((s))){Print(L"EFI_ERROR: " msg L" %d\n", (s)); return (s);}

EFI_STATUS
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
	EFI_STATUS status;

	InitializeLib(ImageHandle, SystemTable);

	Print(L"Popcorn OS " GIT_VERSION L" booting...\n");

	Print(L"Setting console display mode... ");

	EFI_GRAPHICS_OUTPUT_PROTOCOL *gfx_out_proto;
	EFI_GUID gfx_out_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
	status = SystemTable->BootServices->LocateProtocol(&gfx_out_guid, NULL, (void**)&gfx_out_proto);
	check_status(status, "LocateProtocol gfx");

	const uint32_t modes = gfx_out_proto->Mode->MaxMode;
	uint32_t res =
		gfx_out_proto->Mode->Info->HorizontalResolution *
		gfx_out_proto->Mode->Info->VerticalResolution;
	uint32_t best = (uint32_t)-1;

	for (uint32_t i = 0; i < modes; ++i) {
		UINTN size = 0;
		EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info = NULL;
		status = gfx_out_proto->QueryMode(gfx_out_proto, i, &size, &info);
		check_status(status, "QueryMode");

		const uint32_t new_res =
			info->HorizontalResolution *
			info->VerticalResolution;

		if (new_res > res) {
			best = i;
			res = new_res;
		}
	}

	if (best != (uint32_t)-1) {
		status = gfx_out_proto->SetMode(gfx_out_proto, best);
		check_status(status, "SetMode");
		Print(L"*");
	}
	Print(L"%ux%u\n",
		gfx_out_proto->Mode->Info->HorizontalResolution,
		gfx_out_proto->Mode->Info->VerticalResolution);


	Print(L" SystemTable: %x\n", SystemTable);
	if (SystemTable)
		Print(L"      ConOut: %x\n", SystemTable->ConOut);
	if (SystemTable->ConOut)
		Print(L"OutputString: %x\n", SystemTable->ConOut->OutputString);

	while (1) __asm__("hlt");
	return status;
}

