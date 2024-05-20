#include "pch.h"
#include "framework.h"
#include "FauxLauncher.h"
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
#include <utility>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
std::ofstream logFile;                          // Log file

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// Logging function
void Log(const std::string& message) {
    if (logFile.is_open()) {
        logFile << message << std::endl;
    }
}

// Convert std::string to std::wstring
std::wstring StringToWString(const std::string& s) {
    int len;
    int s_len = static_cast<int>(s.size()) + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), s_len, 0, 0);
    std::wstring r(len, L'\0');
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), s_len, &r[0], len);
    return r;
}

// Modify the ExtractResource function to accept a vector of pairs, each containing a resource ID and an output filename
bool ExtractResources(const HMODULE hModule, const std::vector<std::pair<UINT, std::string>>& resources) {
    bool allSucceeded = true;
    for (const auto& [resourceID, outputFilename] : resources) {
        HRSRC hResource = FindResource(hModule, MAKEINTRESOURCE(resourceID), RT_RCDATA);
        if (hResource == NULL) {
            allSucceeded = false;
            Log("Failed to find resource with ID: " + std::to_string(resourceID));
            continue;
        }

        HGLOBAL hLoadedResource = LoadResource(hModule, hResource);
        if (hLoadedResource == NULL) {
            allSucceeded = false;
            Log("Failed to load resource with ID: " + std::to_string(resourceID));
            continue;
        }

        LPVOID lpResourceData = LockResource(hLoadedResource);
        if (lpResourceData == NULL) {
            allSucceeded = false;
            Log("Failed to lock resource with ID: " + std::to_string(resourceID));
            continue;
        }

        DWORD dwResourceSize = SizeofResource(hModule, hResource);
        std::ofstream outputFile(outputFilename, std::ios::binary);
        if (outputFile.is_open()) {
            outputFile.write(reinterpret_cast<const char*>(lpResourceData), dwResourceSize);
            outputFile.close();
            Log("Successfully extracted resource with ID: " + std::to_string(resourceID));
        }
        else {
            allSucceeded = false;
            Log("Failed to open file for resource with ID: " + std::to_string(resourceID));
        }
    }
    return allSucceeded;
}

// Function to run a batch script and wait for it to finish
bool RunScriptAndWait(const std::string& scriptPath) {
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi;
    std::wstring command = StringToWString("cmd.exe /C \"" + scriptPath + "\"");

    if (!CreateProcessW(NULL, &command[0], NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        Log("Failed to create process for script: " + scriptPath);
        return false;
    }

    // Wait until the script finishes
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process and thread handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    Log("Script finished: " + scriptPath);
    return true;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    namespace fs = std::filesystem;

    // Open log file
    logFile.open("FauxLauncher.log", std::ios::out);
    if (!logFile.is_open()) {
        return (INT_PTR)FALSE;
    }

    // Get the path of the executable
    WCHAR buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
    fs::path executablePath(buffer); // Convert directly to fs::path

    // Get the parent directory of the executable
    fs::path directoryPath = executablePath.parent_path();

    // Convert wide string to narrow string
    std::wstring wideDirectoryPath = directoryPath.wstring();
    std::string narrowDirectoryPath(wideDirectoryPath.begin(), wideDirectoryPath.end());

    // Get the path of the temp directory
    char* tempPath = nullptr;
    size_t tempPathSize = 0;
    if (_dupenv_s(&tempPath, &tempPathSize, "TEMP") != 0 || tempPath == nullptr) {
        Log("Failed to get TEMP environment variable.");
        logFile.close();
        return (INT_PTR)FALSE;
    }

    // Define tempDir
    std::filesystem::path tempDir(tempPath);
    tempDir /= "FauxLauncher";
    // Remove old temp directory if it still exists before creating a new one
    if (std::filesystem::exists(tempDir))
    {
        std::filesystem::remove_all(tempDir);
        Log("Removed old temp directory.");
    }
    // Create new temp directory
    std::filesystem::create_directory(tempDir);
    Log("Created new temp directory.");

    // Free the memory allocated by _dupenv_s
    free(tempPath);

    // Output the path of the directory into executablePath.txt in tempDir.
    fs::path outputPath = (tempDir / "executablePath.txt");
    std::ofstream outputFile(outputPath);
    if (outputFile.is_open()) {
        outputFile << narrowDirectoryPath; // Write the narrow string to the file
        outputFile.close();
        Log("The directory path has been written to executablePath.txt in tempDir.");
    }
    else {
        Log("Unable to open executablePath.txt for writing.");
    }

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

        // Change the working directory to the temp directory
        std::filesystem::current_path(tempDir);

        // Run the client_world_backup.bat script and wait for it to finish
        if (!RunScriptAndWait("client_world_backup.bat")) {
            Log("Failed to run or wait for client_world_backup.bat.");
        }
    }
    else {
        Log("One or more resource extractions failed.");
    }

    // Debug statement to confirm script completion
    Log("All scripts have finished running.");

    // Now all scripts have finished, change working directory back to executablePath and remove tempDir
    std::filesystem::current_path(directoryPath);
    Log("Changed working directory back to executable path.");

    // Retry mechanism for deleting tempDir
    const int maxRetries = 5;
    const int retryDelay = 1000; // in milliseconds
    bool success = false;
    for (int attempt = 1; attempt <= maxRetries; ++attempt) {
        std::error_code ec;
        std::filesystem::remove_all(tempDir, ec);
        if (ec) {
            Log("Attempt " + std::to_string(attempt) + " to remove temporary directory failed: " + ec.message());
            std::this_thread::sleep_for(std::chrono::milliseconds(retryDelay));
        }
        else {
            Log("Temporary directory removed successfully on attempt " + std::to_string(attempt));
            success = true;
            break;
        }
    }

    if (!success) {
        Log("Failed to remove temporary directory after " + std::to_string(maxRetries) + " attempts.");
    }

    // Close log file
    logFile.close();

    return (INT_PTR)FALSE;
}