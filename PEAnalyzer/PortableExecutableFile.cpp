/**
 **/

#include "stdafx.h"
#include <Dbghelp.h>
#include <codecvt>
#include <winnt.h>
///
#include "PortableExecutableFile.h"
#pragma comment(lib, "DbgHelp.lib")

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

enum FileMappingStatus {
  kNotOptions,
  kCreateFile,
  kCreateFileMap,
  kMapViewFile
};

class WinMapFile {
private:
  HANDLE hFile = nullptr;
  HANDLE hMapFile = nullptr;
  LPVOID baseAddress = nullptr;
  int status = kNotOptions;

public:
  ~WinMapFile() {
    switch (status) {
    case kMapViewFile:
      UnmapViewOfFile(baseAddress);
    case kCreateFileMap:
      CloseHandle(hMapFile);
    case kCreateFile:
      CloseHandle(hFile);
      break;
    case kNotOptions:
    default:
      break;
    }
  }
  bool Open(const std::wstring &path) {
    hFile = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL,
                        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
      return false;
    status = kCreateFile;
    return true;
  }
  bool CreateFileMap(const wchar_t *name) {
    if (status = kCreateFile) {
      hMapFile = CreateFileMappingW(hFile, NULL, PAGE_READONLY, 0, 0, name);
      if (hMapFile == nullptr)
        return false;
      status = kCreateFileMap;
      return true;
    }
  }
  PVOID MapViewOfFile() {
    baseAddress = ::MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0);
    if (baseAddress == nullptr)
      return nullptr;
    status = kMapViewFile;
    return baseAddress;
  }
  HANDLE Get() { return hFile; }
  void Close() {
    if (hFile) {
      CloseHandle(hFile);
      hFile = nullptr;
    }
  }
};

struct VALUE_STRING {
  int index;
  const wchar_t *str;
};

const VALUE_STRING archList[] = {
	{ PROCESSOR_ARCHITECTURE_INTEL ,L"Win32"},
	{ PROCESSOR_ARCHITECTURE_MIPS, L"MIPS" },
	{ PROCESSOR_ARCHITECTURE_ALPHA, L"Alpha" },
	{ PROCESSOR_ARCHITECTURE_PPC, L"PPC" },
	{ PROCESSOR_ARCHITECTURE_SHX, L"SHX" },
	{ PROCESSOR_ARCHITECTURE_ARM, L"ARM" },
	{ PROCESSOR_ARCHITECTURE_IA64, L"IA64" },
	{ PROCESSOR_ARCHITECTURE_ALPHA64, L"Alpha64" },
	{ PROCESSOR_ARCHITECTURE_MSIL, L"MSIL" },
	{ PROCESSOR_ARCHITECTURE_AMD64, L"Win64" },
	{ PROCESSOR_ARCHITECTURE_IA32_ON_WIN64, L"Wow64" },
	{ PROCESSOR_ARCHITECTURE_NEUTRAL, L"Neutral" },
	{ PROCESSOR_ARCHITECTURE_ARM64, L"ARM64" },
	{ PROCESSOR_ARCHITECTURE_ARM32_ON_WIN64, L"ARM32-Win64" },
	{ PROCESSOR_ARCHITECTURE_IA32_ON_ARM64, L"IA32-ARM64" },
};

const wchar_t *ArchitectureName(int id)
{
	for (auto &x : archList) {
		if (x.index == id)
			return x.str;
	}
	return L"Unknown";
}


const VALUE_STRING machineTable[] = {
    {IMAGE_FILE_MACHINE_UNKNOWN, L"Unknown Machine"},
    {IMAGE_FILE_MACHINE_TARGET_HOST, L"WoW Gest"},
    {IMAGE_FILE_MACHINE_I386, L"Intel 386"},
    {IMAGE_FILE_MACHINE_R3000, L"MIPS little-endian, 0x160 big-endian"},
    {IMAGE_FILE_MACHINE_R4000, L"MIPS little-endian"},
    {IMAGE_FILE_MACHINE_R10000, L"MIPS little-endian"},
    {IMAGE_FILE_MACHINE_WCEMIPSV2, L"MIPS little-endian WCE v2"},
    {IMAGE_FILE_MACHINE_ALPHA, L"Alpha_AXP"},
    {IMAGE_FILE_MACHINE_SH3, L"SH3 little-endian"},
    {IMAGE_FILE_MACHINE_SH3DSP, L"SH3 DSP"},
    {IMAGE_FILE_MACHINE_SH3E, L"SH3E little-endian"},
    {IMAGE_FILE_MACHINE_SH4, L"SH4 little-endian"},
    {IMAGE_FILE_MACHINE_SH5, L"SH5"},
    {IMAGE_FILE_MACHINE_ARM, L"ARM Little-Endian"},
    {IMAGE_FILE_MACHINE_THUMB, L"ARM Thumb/Thumb-2 Little-Endian"},
    {IMAGE_FILE_MACHINE_ARMNT, L"ARM Thumb-2 Little-Endian"},
    {IMAGE_FILE_MACHINE_AM33, L"Matsushita AM33"},
    {IMAGE_FILE_MACHINE_POWERPC, L"IBM PowerPC Little-Endian"},
    {IMAGE_FILE_MACHINE_POWERPCFP, L"IBM PowerPC  (FP support)"},
    {IMAGE_FILE_MACHINE_IA64, L"Intel IA64"},
    {IMAGE_FILE_MACHINE_MIPS16, L"MIPS"},
    {IMAGE_FILE_MACHINE_ALPHA64, L"ALPHA64"},
    {IMAGE_FILE_MACHINE_MIPSFPU, L"MIPS"},
    {IMAGE_FILE_MACHINE_MIPSFPU16, L"MIPS"},
    {IMAGE_FILE_MACHINE_TRICORE, L"Infineon"},
    {IMAGE_FILE_MACHINE_CEF, L"IMAGE_FILE_MACHINE_CEF"},
    {IMAGE_FILE_MACHINE_EBC, L"EFI Byte Code"},
    {IMAGE_FILE_MACHINE_AMD64, L"AMD64 (K8)"},
    {IMAGE_FILE_MACHINE_M32R, L"M32R little-endian"},
    {IMAGE_FILE_MACHINE_ARM64, L"ARM64 Little-Endian"},
    {IMAGE_FILE_MACHINE_CEE, L"IMAGE_FILE_MACHINE_CEE"}};

const VALUE_STRING subsystemTable[] = {
    {IMAGE_SUBSYSTEM_UNKNOWN, L"Unknown subsystem"},
    {IMAGE_SUBSYSTEM_NATIVE, L"Native"}, // not require subsystem
    {IMAGE_SUBSYSTEM_WINDOWS_GUI, L"Windows GUI"},
    {IMAGE_SUBSYSTEM_WINDOWS_CUI, L"Windows CUI"},
    {IMAGE_SUBSYSTEM_OS2_CUI, L"OS/2  CUI"},
    {IMAGE_SUBSYSTEM_POSIX_CUI, L"Posix character subsystem"},
    {IMAGE_SUBSYSTEM_NATIVE_WINDOWS, L"Native Win9x driver"},
    {IMAGE_SUBSYSTEM_WINDOWS_CE_GUI, L"Windows CE subsystem"},
    {IMAGE_SUBSYSTEM_EFI_APPLICATION, L"EFI Application"},
    {IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER, L"EFI Boot Service Driver"},
    {IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER, L"EFI Runtime Driver"},
    {IMAGE_SUBSYSTEM_EFI_ROM, L"EFI ROM"},
    {IMAGE_SUBSYSTEM_XBOX, L"Xbox system"},
    {IMAGE_SUBSYSTEM_WINDOWS_BOOT_APPLICATION, L"Windows Boot Application"},
    {IMAGE_SUBSYSTEM_XBOX_CODE_CATALOG, L"XBOX Code Catalog"}};

PortableExecutableFile::PortableExecutableFile(const std::wstring &mPath)
    : mPath_(mPath) {
  subsystem = subsystemTable[0].str;
  clrMessage = L"Native Executable File";
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
#define IMAGE_REL_ARM64_ADDR32          0x0001  // 32 bit address. Review! do we
need it? #define IMAGE_REL_ARM64_ADDR32NB        0x0002  // 32 bit address w/o
image base (RVA: for Data/PData/XData) #define IMAGE_REL_ARM64_BRANCH26
0x0003  // 26 bit offset << 2 & sign ext. for B & BL #define
IMAGE_REL_ARM64_PAGEBASE_REL21  0x0004  // ADRP #define IMAGE_REL_ARM64_REL21
0x0005  // ADR #define IMAGE_REL_ARM64_PAGEOFFSET_12A  0x0006  // ADD/ADDS
(immediate) with zero shift, for page offset #define
IMAGE_REL_ARM64_PAGEOFFSET_12L  0x0007  // LDR (indexed, unsigned immediate),
for page offset #define IMAGE_REL_ARM64_SECREL          0x0008  // Offset within
section #define IMAGE_REL_ARM64_SECREL_LOW12A   0x0009  // ADD/ADDS (immediate)
with zero shift, for bit 0:11 of section offset #define
IMAGE_REL_ARM64_SECREL_HIGH12A  0x000A  // ADD/ADDS (immediate) with zero shift,
for bit 12:23 of section offset #define IMAGE_REL_ARM64_SECREL_LOW12L   0x000B
// LDR (indexed, unsigned immediate), for bit 0:11 of section offset #define
IMAGE_REL_ARM64_TOKEN           0x000C #define IMAGE_REL_ARM64_SECTION
0x000D  // Section table index #define IMAGE_REL_ARM64_ADDR64          0x000E
// 64 bit address

//
*/

/*
// Code Integrity in loadconfig (CI)
//

typedef struct _IMAGE_LOAD_CONFIG_CODE_INTEGRITY {
WORD    Flags;          // Flags to indicate if CI information is available,
etc. WORD    Catalog;        // 0xFFFF means not available DWORD
CatalogOffset; DWORD   Reserved;       // Additional bitmask to be defined later
} IMAGE_LOAD_CONFIG_CODE_INTEGRITY, *PIMAGE_LOAD_CONFIG_CODE_INTEGRITY;

//
*/

bool PortableExecutableFile::Analyzer() {
  WinMapFile winfile;
  if (!winfile.Open(mPath_)) {
    return false;
  }
  LARGE_INTEGER largeFile;
  if (!GetFileSizeEx(winfile.Get(), &largeFile))
    return false;
  if (largeFile.QuadPart < sizeof(IMAGE_DOS_HEADER))
    return false;
  if (!winfile.CreateFileMap(L"PEAnalyzer.Executeable.MAP"))
    return false;
  char *baseAddress = (char *)winfile.MapViewOfFile();
  IMAGE_DOS_HEADER *pDOSHeader =
      reinterpret_cast<IMAGE_DOS_HEADER *>(baseAddress);
  IMAGE_NT_HEADERS *pNTHeader =
      reinterpret_cast<IMAGE_NT_HEADERS *>(baseAddress + pDOSHeader->e_lfanew);
  if (largeFile.QuadPart <
      (LONGLONG)(sizeof(IMAGE_DOS_HEADER) + pDOSHeader->e_lfanew +
                 sizeof(IMAGE_NT_HEADERS)))
    return false;
  union SigMask {
    DWORD dw;
    char c[4];
  };
  SigMask sigmark;
  sigmark.dw = pNTHeader->Signature;
  for (auto &c : sigmark.c) {
    magicStr.push_back(c);
  }
  for (auto &m : machineTable) {
    if (m.index == pNTHeader->FileHeader.Machine) {
      machine = m.str;
      break;
    }
  }
  for (auto &sub : subsystemTable) {
    if (sub.index == pNTHeader->OptionalHeader.Subsystem) {
      subsystem = sub.str;
    }
  }
  auto var = pNTHeader->FileHeader.Characteristics;
  if ((pNTHeader->FileHeader.Characteristics & IMAGE_FILE_DLL) ==
      IMAGE_FILE_DLL) {
    mCharacteristics = L"Dynamic Link Library";
  } else if ((pNTHeader->FileHeader.Characteristics & IMAGE_FILE_SYSTEM) ==
             IMAGE_FILE_SYSTEM) {
    mCharacteristics = L"System File";
  } else if ((pNTHeader->FileHeader.Characteristics &
              IMAGE_FILE_EXECUTABLE_IMAGE) == IMAGE_FILE_EXECUTABLE_IMAGE) {
    mCharacteristics = L"Executable File";
  } else {
    mCharacteristics = std::wstring(L"Characteristics value: ") +
                       std::to_wstring(pNTHeader->FileHeader.Characteristics);
  }
  wsprintfW(linkVersion, L"%d.%d", pNTHeader->OptionalHeader.MajorLinkerVersion,
            pNTHeader->OptionalHeader.MinorLinkerVersion);
  wsprintfW(osVersion, L"%d.%d",
            pNTHeader->OptionalHeader.MajorOperatingSystemVersion,
            pNTHeader->OptionalHeader.MinorOperatingSystemVersion);
  wsprintfW(subsystemVersion, L"%d.%d",
            pNTHeader->OptionalHeader.MajorSubsystemVersion,
            pNTHeader->OptionalHeader.MinorSubsystemVersion);
  wsprintfW(imageVersion, L"%d.%d", pNTHeader->OptionalHeader.MajorImageVersion,
            pNTHeader->OptionalHeader.MinorImageVersion);
  auto self = &__ImageBase;
  auto selfNTHeader =
      reinterpret_cast<IMAGE_NT_HEADERS *>((char *)self + self->e_lfanew);
  if (selfNTHeader->FileHeader.Machine != pNTHeader->FileHeader.Machine) {
    clrMessage = L"PEAnaylzer's Architecture is different with Input PE "
                 L"File,cannot check CLR";
    return true;
  }
  IMAGE_DATA_DIRECTORY *entry =
      &(pNTHeader->OptionalHeader)
           .DataDirectory[IMAGE_DIRECTORY_ENTRY_COMHEADER];
  if (entry->Size != sizeof(IMAGE_COR20_HEADER)) {
    return true;
  }
  auto va = ImageRvaToVa(pNTHeader, baseAddress, entry->VirtualAddress, 0);
  if ((char *)va - baseAddress > largeFile.QuadPart)
    return false;
  IMAGE_COR20_HEADER *pCLRHeader = reinterpret_cast<IMAGE_COR20_HEADER *>(va);
  char *pMetaDataAddress = reinterpret_cast<char *>(ImageRvaToVa(
      pNTHeader, baseAddress, pCLRHeader->MetaData.VirtualAddress, 0));
  if (pMetaDataAddress - baseAddress > largeFile.QuadPart)
    return false;
  char *buildMessage = pMetaDataAddress + 16;
  std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
  clrMessage = conv.from_bytes(buildMessage);
  return true;
}