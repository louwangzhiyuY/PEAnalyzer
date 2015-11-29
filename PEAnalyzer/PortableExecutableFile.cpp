/**
**/
#include "stdafx.h"
#include <winnt.h>
#include "PortableExecutableFile.h"

class WinFile {
private:
	HANDLE hFile;
public:
	~WinFile()
	{
		if (hFile) {
			CloseHandle(hFile);
		}
	}
	bool Open(const std::wstring &path)
	{
		hFile = CreateFileW(path.c_str(),
							GENERIC_READ,
							FILE_SHARE_READ,
							NULL,
							OPEN_ALWAYS,
							FILE_ATTRIBUTE_NORMAL,
							NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			return false;
		return true;
	}
	HANDLE Get()
	{
		return hFile;
	}
	void Close()
	{
		if (hFile) {
			CloseHandle(hFile);
			hFile = nullptr;
		}
	}
};

struct VALUIE_STRING {
	int index;
	const wchar_t *str;
};

const  VALUIE_STRING signatureTable[] = {
	{ IMAGE_DOS_SIGNATURE, L"MZ" },
	{ IMAGE_OS2_SIGNATURE, L"NE" },
	{ IMAGE_OS2_SIGNATURE_LE, L"LE" },
	{ IMAGE_NT_SIGNATURE, L"PE00" },
	{ 0, L"Unknown " }
};

const  VALUIE_STRING machineTable[] = {
	{ IMAGE_FILE_MACHINE_UNKNOWN, L"Unknown Machine" },
	{ IMAGE_FILE_MACHINE_I386, L"Intel 386" },
	{ IMAGE_FILE_MACHINE_R3000, L"MIPS little-endian, 0x160 big-endian" },
	{ IMAGE_FILE_MACHINE_R4000, L"MIPS little-endian" },
	{ IMAGE_FILE_MACHINE_R10000, L"MIPS little-endian" },
	{ IMAGE_FILE_MACHINE_WCEMIPSV2, L"MIPS little-endian WCE v2" },
	{ IMAGE_FILE_MACHINE_ALPHA, L"Alpha_AXP" },
	{ IMAGE_FILE_MACHINE_SH3, L"SH3 little-endian" },
	{ IMAGE_FILE_MACHINE_SH3DSP, L"SH3 DSP" },
	{ IMAGE_FILE_MACHINE_SH3E, L"SH3E little-endian" },
	{ IMAGE_FILE_MACHINE_SH4, L"SH4 little-endian" },
	{ IMAGE_FILE_MACHINE_SH5, L"SH5" },
	{ IMAGE_FILE_MACHINE_ARM, L"ARM Little-Endian" },
	{ IMAGE_FILE_MACHINE_THUMB, L"ARM Thumb/Thumb-2 Little-Endian" },
	{ IMAGE_FILE_MACHINE_ARMNT, L"ARM Thumb-2 Little-Endian" },
	{ IMAGE_FILE_MACHINE_AM33, L"Matsushita AM33" },
	{ IMAGE_FILE_MACHINE_POWERPC, L"IBM PowerPC Little-Endian" },
	{ IMAGE_FILE_MACHINE_POWERPCFP, L"IBM PowerPC  (FP support)" },
	{ IMAGE_FILE_MACHINE_IA64, L"Intel IA64" },
	{ IMAGE_FILE_MACHINE_MIPS16, L"MIPS" },
	{ IMAGE_FILE_MACHINE_ALPHA64, L"ALPHA64" },
	{ IMAGE_FILE_MACHINE_MIPSFPU, L"MIPS" },
	{ IMAGE_FILE_MACHINE_MIPSFPU16, L"MIPS" },
	{ IMAGE_FILE_MACHINE_TRICORE, L"Infineon" },
	{ IMAGE_FILE_MACHINE_CEF, L"IMAGE_FILE_MACHINE_CEF" },
	{ IMAGE_FILE_MACHINE_EBC, L"EFI Byte Code" },
	{ IMAGE_FILE_MACHINE_AMD64, L"AMD64 (K8)" },
	{ IMAGE_FILE_MACHINE_M32R, L"M32R little-endian" },
	{ IMAGE_FILE_MACHINE_ARM64, L"ARM64 Little-Endian" },
	{ IMAGE_FILE_MACHINE_CEE, L"IMAGE_FILE_MACHINE_CEE" }
};

const VALUIE_STRING subsystemTable[] = {
	{ IMAGE_SUBSYSTEM_UNKNOWN, L"Unknown subsystem" },
	{ IMAGE_SUBSYSTEM_NATIVE, L"Native" }, //not require subsystem
	{ IMAGE_SUBSYSTEM_WINDOWS_GUI, L"Windows GUI" },
	{ IMAGE_SUBSYSTEM_WINDOWS_CUI, L"Windows CUI" },
	{ IMAGE_SUBSYSTEM_OS2_CUI, L"OS/2  CUI" },
	{ IMAGE_SUBSYSTEM_POSIX_CUI, L"Posix character subsystem" },
	{ IMAGE_SUBSYSTEM_NATIVE_WINDOWS, L"Native Win9x driver" },
	{ IMAGE_SUBSYSTEM_WINDOWS_CE_GUI, L"Windows CE subsystem" },
	{ IMAGE_SUBSYSTEM_EFI_APPLICATION, L"EFI Application" },
	{ IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER, L"EFI Boot Service Driver" },
	{ IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER, L"EFI Runtime Driver" },
	{ IMAGE_SUBSYSTEM_EFI_ROM, L"EFI ROM" },
	{ IMAGE_SUBSYSTEM_XBOX, L"Xbox system" },
	{ IMAGE_SUBSYSTEM_WINDOWS_BOOT_APPLICATION, L"Windows Boot Application" },
	{ IMAGE_SUBSYSTEM_XBOX_CODE_CATALOG, L"XBOX Code Catalog" }
};

PortableExecutableFile::PortableExecutableFile(const std::wstring &mPath) :mPath_(mPath)
{
	subsystem = subsystemTable[0].str;
}
/*
typedef enum _SID_NAME_USE {
SidTypeUser = 1,
SidTypeGroup,
SidTypeDomain,
SidTypeAlias,
SidTypeWellKnownGroup,
SidTypeDeletedAccount,
SidTypeInvalid,
SidTypeUnknown,
SidTypeComputer,
SidTypeLabel,
SidTypeLogonSession
} SID_NAME_USE, *PSID_NAME_USE;
*/

/*
//
// ARM64 relocations types.
//

#define IMAGE_REL_ARM64_ABSOLUTE        0x0000  // No relocation required
#define IMAGE_REL_ARM64_ADDR32          0x0001  // 32 bit address. Review! do we need it?
#define IMAGE_REL_ARM64_ADDR32NB        0x0002  // 32 bit address w/o image base (RVA: for Data/PData/XData)
#define IMAGE_REL_ARM64_BRANCH26        0x0003  // 26 bit offset << 2 & sign ext. for B & BL
#define IMAGE_REL_ARM64_PAGEBASE_REL21  0x0004  // ADRP
#define IMAGE_REL_ARM64_REL21           0x0005  // ADR
#define IMAGE_REL_ARM64_PAGEOFFSET_12A  0x0006  // ADD/ADDS (immediate) with zero shift, for page offset
#define IMAGE_REL_ARM64_PAGEOFFSET_12L  0x0007  // LDR (indexed, unsigned immediate), for page offset
#define IMAGE_REL_ARM64_SECREL          0x0008  // Offset within section
#define IMAGE_REL_ARM64_SECREL_LOW12A   0x0009  // ADD/ADDS (immediate) with zero shift, for bit 0:11 of section offset
#define IMAGE_REL_ARM64_SECREL_HIGH12A  0x000A  // ADD/ADDS (immediate) with zero shift, for bit 12:23 of section offset
#define IMAGE_REL_ARM64_SECREL_LOW12L   0x000B  // LDR (indexed, unsigned immediate), for bit 0:11 of section offset
#define IMAGE_REL_ARM64_TOKEN           0x000C
#define IMAGE_REL_ARM64_SECTION         0x000D  // Section table index
#define IMAGE_REL_ARM64_ADDR64          0x000E  // 64 bit address

//
*/

/*
// Code Integrity in loadconfig (CI)
//

typedef struct _IMAGE_LOAD_CONFIG_CODE_INTEGRITY {
WORD    Flags;          // Flags to indicate if CI information is available, etc.
WORD    Catalog;        // 0xFFFF means not available
DWORD   CatalogOffset;
DWORD   Reserved;       // Additional bitmask to be defined later
} IMAGE_LOAD_CONFIG_CODE_INTEGRITY, *PIMAGE_LOAD_CONFIG_CODE_INTEGRITY;

//
*/

bool PortableExecutableFile::Analyzer()
{
	WinFile wfile;
	if (!wfile.Open(mPath_)) {
		return false;
	}
	DWORD bytes_read = 0;
	IMAGE_DOS_HEADER dos_header;
	IMAGE_NT_HEADERS nt_header;
	ReadFile(wfile.Get(), &dos_header, sizeof(dos_header), &bytes_read, NULL);
	if (bytes_read != sizeof(dos_header)) {
		return false;
	}
	SetFilePointer(wfile.Get(), dos_header.e_lfanew, NULL, FILE_BEGIN);
	ReadFile(wfile.Get(), &nt_header, sizeof(nt_header), &bytes_read, NULL);
	if (bytes_read != sizeof(nt_header)) {
		return false;
	}
	for (auto &s : signatureTable) {
		if (s.index == nt_header.Signature) {
			magicStr = s.str;
			break;
		}
	}
	for (auto &m : machineTable) {
		if (m.index == nt_header.FileHeader.Machine) {
			machine = m.str;
			break;
		}
	}
	for (auto &sub : subsystemTable) {
		if (sub.index == nt_header.OptionalHeader.Subsystem) {
			subsystem = sub.str;
		}
	}
	auto var = nt_header.FileHeader.Characteristics;
	if ((nt_header.FileHeader.Characteristics&IMAGE_FILE_DLL) == IMAGE_FILE_DLL) {
		mCharacteristics = L"Dynamic Link Library";
	}else if ((nt_header.FileHeader.Characteristics&IMAGE_FILE_SYSTEM) == IMAGE_FILE_SYSTEM) {
		mCharacteristics = L"System File";
	}else if ((nt_header.FileHeader.Characteristics&IMAGE_FILE_EXECUTABLE_IMAGE) == IMAGE_FILE_EXECUTABLE_IMAGE) {
		mCharacteristics = L"Executable File";
	} else {
		mCharacteristics = std::wstring(L"Characteristics value: ") + std::to_wstring(nt_header.FileHeader.Characteristics);
	}

	wsprintfW(linkVersion, L"%d.%d", 
			  nt_header.OptionalHeader.MajorLinkerVersion,
			  nt_header.OptionalHeader.MinorLinkerVersion);
	wsprintfW(osVersion, L"%d.%d",
			  nt_header.OptionalHeader.MajorOperatingSystemVersion,
			  nt_header.OptionalHeader.MinorOperatingSystemVersion);
	wsprintfW(subsystemVersion, L"%d.%d",
			  nt_header.OptionalHeader.MajorSubsystemVersion,
			  nt_header.OptionalHeader.MinorSubsystemVersion);
	wsprintfW(imageVersion, L"%d.%d",
			  nt_header.OptionalHeader.MajorImageVersion,
			  nt_header.OptionalHeader.MinorImageVersion);
	wsprintfW(entryPoint, L"0x%x", nt_header.OptionalHeader.AddressOfEntryPoint);
	return true;
}