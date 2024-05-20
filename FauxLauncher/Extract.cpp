#include "pch.h"
#include "framework.h"
#include "FauxLauncher.h"
// User defined
#include <windows.h>
#include <fstream>

bool ExtractResource(const HMODULE hModule, const UINT resourceID, const LPCSTR outputFilename) {
    HRSRC hResource = FindResource(hModule, MAKEINTRESOURCE(resourceID), RT_RCDATA);
    if (hResource == NULL) {
        return false;
    }

    HGLOBAL hLoadedResource = LoadResource(hModule, hResource);
    if (hLoadedResource == NULL) {
        return false;
    }

    LPVOID lpResourceData = LockResource(hLoadedResource);
    if (lpResourceData == NULL) {
        return false;
    }

    DWORD dwResourceSize = SizeofResource(hModule, hResource);
    std::ofstream outputFile(outputFilename, std::ios::binary);
    outputFile.write(reinterpret_cast<const char*>(lpResourceData), dwResourceSize);
    outputFile.close();

    return true;
}

int main() {
    HMODULE hModule = GetModuleHandle(NULL); // Use LoadLibrary if it's not the current executable

    // Replace 'YOUR_RESOURCE_ID' with the actual resource ID
    // Replace 'YOUR_OUTPUT_FILENAME' with the desired output file name
    if (ExtractResource(hModule, 255, "Minecraft.lnk")) {
        // Resource extraction succeeded
    }
    else {
        // Resource extraction failed
    }

    return 0;
}
