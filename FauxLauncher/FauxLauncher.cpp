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
#include <shellapi.h>

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

// Function prototype for ExtractResource
bool ExtractResource(const HMODULE hModule, const UINT resourceID, const LPCSTR outputFilename);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

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

    // Replace 'YOUR_RESOURCE_ID' with the actual resource ID
    // Replace 'YOUR_OUTPUT_FILENAME' with the desired output file name
    if (ExtractResource(hInstance, 255, "Minecraft.lnk")) {
        // Resource extraction succeeded
    }
    else {
        // Resource extraction failed
    }

    return (INT_PTR)FALSE;
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

    return true;
}