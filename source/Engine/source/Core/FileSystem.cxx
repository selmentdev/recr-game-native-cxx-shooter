//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core/FileSystem.hxx>
#include <fstream>
#include <Windows.h>

namespace Core
{
    bool FileSystem::Load(std::vector<uint8_t>& result, const std::string& path) noexcept
    {
        //
        // Create file handle for that path.
        //
        ::HANDLE file = ::CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

        if (file != nullptr && file != INVALID_HANDLE_VALUE)
        {
            //
            // Close file on exit.
            //
            struct HackityHack
            {
            private:
                ::HANDLE m_Handle;

            public:
                HackityHack(::HANDLE handle) noexcept
                    : m_Handle{ handle }
                {
                }
                ~HackityHack() noexcept
                {
                    ::CloseHandle(m_Handle);
                }
            } closeFile{ file };

            ::FILE_STANDARD_INFO fileInfo{};

            //
            // Try access file information.
            //
            if (::GetFileInformationByHandleEx(file, FileStandardInfo, &fileInfo, sizeof(fileInfo)) == FALSE)
            {
                return false;
            }

            //
            // Don't support large files.
            //
            if (fileInfo.EndOfFile.HighPart > 0)
            {
                return false;
            }

            //
            // Resize buffer.
            //
            result.resize(fileInfo.EndOfFile.LowPart);

            ::DWORD dwRead{};

            //
            // And read data.
            //
            if (::ReadFile(file, result.data(), fileInfo.EndOfFile.LowPart, &dwRead, nullptr) == FALSE)
            {
                return false;
            }

            //
            // Validate if we read whole file.
            //
            if (dwRead < fileInfo.EndOfFile.LowPart)
            {
                return false;
            }
            
            return true;
        }

        return false;
    }
}