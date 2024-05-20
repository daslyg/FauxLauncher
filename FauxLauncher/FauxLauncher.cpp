// FauxLauncher.cpp : Defines the entry point for the application.
//

#include "pch.h"
#include "framework.h"
#include "FauxLauncher.h"
// User defined
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <system_error>
#include <windows.h>

#include <vector>
#include <utility> // For std::pair


#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// Modify the ExtractResource function to accept a vector of pairs, each containing a resource ID and an output filename
bool ExtractResources(const HMODULE hModule, const std::vector<std::pair<UINT, std::string>>& resources) {
    bool allSucceeded = true;
    for (const auto& [resourceID, outputFilename] : resources) {
        HRSRC hResource = FindResource(hModule, MAKEINTRESOURCE(resourceID), RT_RCDATA);
        if (hResource == NULL) {
            allSucceeded = false;
            continue;
        }

        HGLOBAL hLoadedResource = LoadResource(hModule, hResource);
        if (hLoadedResource == NULL) {
            allSucceeded = false;
            continue;
        }

        LPVOID lpResourceData = LockResource(hLoadedResource);
        if (lpResourceData == NULL) {
            allSucceeded = false;
            continue;
        }

        DWORD dwResourceSize = SizeofResource(hModule, hResource);
        std::ofstream outputFile(outputFilename, std::ios::binary);
        if (outputFile.is_open()) {
            outputFile.write(reinterpret_cast<const char*>(lpResourceData), dwResourceSize);
            outputFile.close();
        }
        else {
            allSucceeded = false;
        }
    }
    return allSucceeded;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    namespace fs = std::filesystem;

    // Get the path of the executable
    WCHAR buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
    fs::path executablePath(buffer); // Convert directly to fs::path

    // Output the path of the executable into a output.txt in outdir
    fs::path outputPath = "executablePath.txt";
    std::ofstream outputFile(outputPath);
    if (outputFile.is_open()) {
        outputFile << executablePath;
        outputFile.close();
        std::cout << "The executable path has been written to output.txt in outdir." << std::endl;
    }
    else {
        std::cerr << "Unable to open output.txt for writing." << std::endl;
    }

    // Get the path of the temp directory
    char* tempPath = nullptr;
    size_t tempPathSize = 0;
    if (_dupenv_s(&tempPath, &tempPathSize, "TEMP") != 0 || tempPath == nullptr) {
        std::cerr << "Failed to get TEMP environment variable." << std::endl;
        return (INT_PTR)FALSE;
    }

    // Create a new directory within the temp directory
    std::filesystem::path tempDir(tempPath);
    tempDir /= "MyAppTempDir";
    std::filesystem::create_directory(tempDir);

    // Free the memory allocated by _dupenv_s
    free(tempPath);

    // Define a list of resources to extract
    std::vector<std::pair<UINT, std::string>> resourcesToExtract = {
        {255, (tempDir / "Minecraft.lnk").string()},
        // Add more resources here
        {256, (tempDir / "client_world_backup.bat").string()},
        {257, (tempDir / "client_assets_backup.bat").string()}
        // etc.
    };

    // Extract all resources
    if (ExtractResources(hInstance, resourcesToExtract)) {
        // All resources extracted successfully

        // Run the client_world_backup.bat script
        std::string command = (tempDir / "client_world_backup.bat").string();
        system(command.c_str());
    }
    else {
        // One or more resource extractions failed
    }
}

// Definition of ExtractResource
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

    return (INT_PTR)FALSE;
}