#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>

std::wstring GetProcessorArchitecture() {
    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);

    switch (systemInfo.wProcessorArchitecture) {
        case PROCESSOR_ARCHITECTURE_INTEL:
            return L"Intel x86";
        case PROCESSOR_ARCHITECTURE_AMD64:
            return L"AMD64";
        case PROCESSOR_ARCHITECTURE_ARM:
            return L"ARM";
        default:
            return L"Desconocido";
    }
}

std::vector<std::wstring> GetInstalledPrograms() {
    std::vector<std::wstring> installedPrograms;
    HKEY hKey;

    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD index = 0;
        WCHAR subkey[1024];
        DWORD subkeySize = sizeof(subkey);

        while (RegEnumKeyExW(hKey, index, subkey, &subkeySize, NULL, NULL, NULL, NULL) != ERROR_NO_MORE_ITEMS) {
            HKEY subKeyHandle;

            if (RegOpenKeyExW(hKey, subkey, 0, KEY_READ, &subKeyHandle) == ERROR_SUCCESS) {
                WCHAR displayName[1024];
                DWORD displayNameSize = sizeof(displayName);

                if (RegQueryValueExW(subKeyHandle, L"DisplayName", NULL, NULL, (LPBYTE)displayName, &displayNameSize) == ERROR_SUCCESS) {
                    installedPrograms.push_back(displayName);
                }

                RegCloseKey(subKeyHandle);
            }

            index++;
            subkeySize = sizeof(subkey);
        }

        RegCloseKey(hKey);
    }

    return installedPrograms;
}

int main() {
    WCHAR username[256];
    DWORD size = 256;

    if (GetUserNameW(username, &size)) {
        std::wcout << L"Usuario: " << username << std::endl;
    } else {
        std::wcerr << L"No se pudo obtener el nombre de usuario." << std::endl;
    }

    MEMORYSTATUSEX memoryStatus;
    memoryStatus.dwLength = sizeof(memoryStatus);
    GlobalMemoryStatusEx(&memoryStatus);

    std::wcout << L"RAM total: " << memoryStatus.ullTotalPhys / (1024 * 1024) << L" MB" << std::endl;
    std::wcout << L"RAM disponible: " << memoryStatus.ullAvailPhys / (1024 * 1024) << L" MB" << std::endl;

    ULARGE_INTEGER freeBytesAvailable, totalNumberOfBytes, totalNumberOfFreeBytes;
    GetDiskFreeSpaceExW(NULL, &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes);

    std::wcout << L"Espacio utilizado en disco: " << (totalNumberOfBytes.QuadPart - totalNumberOfFreeBytes.QuadPart) / (1024 * 1024) << L" MB" << std::endl;

    std::wcout << L"Tipo de procesador: " << GetProcessorArchitecture() << std::endl;

    std::vector<std::wstring> installedPrograms = GetInstalledPrograms();

    std::wcout << L"Programas instalados:" << std::endl;
    for (const std::wstring& program : installedPrograms) {
        std::wcout << program << std::endl;
    }

    return 0;
}
