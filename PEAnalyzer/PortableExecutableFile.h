

#ifndef PORTABLE_EXECUTABLE_FILE_H
#define PORTABLE_EXECUTABLE_FILE_H
#include <string>


#ifndef PROCESSOR_ARCHITECTURE_ARM64
#define PROCESSOR_ARCHITECTURE_ARM64            12
#endif
#ifndef PROCESSOR_ARCHITECTURE_ARM32_ON_WIN64
#define PROCESSOR_ARCHITECTURE_ARM32_ON_WIN64   13
#endif
#ifndef PROCESSOR_ARCHITECTURE_IA32_ON_ARM64
#define PROCESSOR_ARCHITECTURE_IA32_ON_ARM64    14
#endif

#ifndef IMAGE_FILE_MACHINE_TARGET_HOST
#define IMAGE_FILE_MACHINE_TARGET_HOST        0x0001 // Useful for indicating we want to interact with the host and not a WoW guest.
#endif

/// #define PROCESSOR_ARCHITECTURE_ARM32_ON_WIN64   13
#ifndef IMAGE_FILE_MACHINE_ARM64
//// IMAGE_FILE_MACHINE_ARM64 is Windows
#define IMAGE_FILE_MACHINE_ARM64 0xAA64 // ARM64 Little-Endian
#endif
#ifndef IMAGE_SUBSYSTEM_XBOX_CODE_CATALOG
#define IMAGE_SUBSYSTEM_XBOX_CODE_CATALOG 17 // XBOX Code Catalog
#endif

typedef enum ReplacesGeneralNumericDefines {
// Directory entry macro for CLR data.
#ifndef IMAGE_DIRECTORY_ENTRY_COMHEADER
  IMAGE_DIRECTORY_ENTRY_COMHEADER = 14,
#endif // IMAGE_DIRECTORY_ENTRY_COMHEADER
} ReplacesGeneralNumericDefines;

const wchar_t *ArchitectureName(int id);

class PortableExecutableFile {
private:
  std::wstring mPath_;
  std::wstring magicStr;
  std::wstring machine;
  std::wstring subsystem;
  std::wstring mCharacteristics;
  std::wstring clrMessage;
  wchar_t linkVersion[16];
  wchar_t osVersion[16];
  wchar_t subsystemVersion[16];
  wchar_t imageVersion[16];

public:
  PortableExecutableFile(const std::wstring &mPath);
  bool Analyzer();
  const std::wstring &GetSignature() { return magicStr; }
  const std::wstring &GetMachine() { return machine; }
  const std::wstring &GetSubSystem() { return subsystem; }
  const std::wstring &GetCharacteristics() { return mCharacteristics; }
  const std::wstring &GetCLRMessage() { return clrMessage; }
  const wchar_t *GetLinkerVersion() { return linkVersion; }
  const wchar_t *GetOSVersion() { return osVersion; }
  const wchar_t *GetSubSystemVersion() { return subsystemVersion; }
  const wchar_t *GetImageVersion() { return imageVersion; }
};

#endif
