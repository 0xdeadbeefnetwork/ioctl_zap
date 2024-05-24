#include <windows.h>
#include <iostream>
#include <string>
#include <vector>

void fuzzIoctl(const std::wstring& devicePath) {
    HANDLE hDevice = CreateFile(
        devicePath.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hDevice == INVALID_HANDLE_VALUE) {
        std::wcerr << L"Failed to open device: " << devicePath << L" with error: " << GetLastError() << std::endl;
        return;
    }

    // Buffers for input and output
    std::vector<BYTE> inputBuffer(0x10000);  // 64 KB buffer
    std::vector<BYTE> outputBuffer(0x10000); // 64 KB buffer
    DWORD bytesReturned;

    for (DWORD ioctlCode = 0x00000000; ioctlCode <= 0xFFFFFFFF; ioctlCode++) {
        // Trying various buffer sizes including edge cases
        std::vector<DWORD> bufferSizes = { 0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536 };

        for (DWORD bufferSize : bufferSizes) {
            // Trying IOCTL with METHOD_BUFFERED
            if (DeviceIoControl(
                hDevice,
                ioctlCode,
                inputBuffer.data(),
                bufferSize,
                outputBuffer.data(),
                bufferSize,
                &bytesReturned,
                NULL
            )) {
                std::wcout << L"Successful IOCTL: 0x" << std::hex << ioctlCode << L" with buffer size: " << std::dec << bufferSize << std::endl;
            }

            // Trying IOCTL with METHOD_IN_DIRECT
            if (DeviceIoControl(
                hDevice,
                ioctlCode,
                inputBuffer.data(),
                bufferSize,
                outputBuffer.data(),
                bufferSize,
                &bytesReturned,
                NULL
            )) {
                std::wcout << L"Successful IOCTL: 0x" << std::hex << ioctlCode << L" with buffer size: " << std::dec << bufferSize << std::endl;
            }

            // Trying IOCTL with METHOD_OUT_DIRECT
            if (DeviceIoControl(
                hDevice,
                ioctlCode,
                inputBuffer.data(),
                bufferSize,
                outputBuffer.data(),
                bufferSize,
                &bytesReturned,
                NULL
            )) {
                std::wcout << L"Successful IOCTL: 0x" << std::hex << ioctlCode << L" with buffer size: " << std::dec << bufferSize << std::endl;
            }

            // Trying IOCTL with METHOD_NEITHER
            if (DeviceIoControl(
                hDevice,
                ioctlCode,
                inputBuffer.data(),
                bufferSize,
                outputBuffer.data(),
                bufferSize,
                &bytesReturned,
                NULL
            )) {
                std::wcout << L"Successful IOCTL: 0x" << std::hex << ioctlCode << L" with buffer size: " << std::dec << bufferSize << std::endl;
            }
        }
    }

    CloseHandle(hDevice);
}

std::wstring stringToWstring(const std::string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " \\\\.\\[device]\n IOCTL ZAP by _SiCK [AFFLICTED INTELLIGENCE LLC]\n https://afflicted.sh/" << std::endl;
        return 1;
    }

    std::string devicePath = argv[1];
    fuzzIoctl(stringToWstring(devicePath));

    return 0;
}
