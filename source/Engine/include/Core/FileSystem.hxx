#ifndef INCLUDED_CORE_FILESYSTEM_HXX
#define INCLUDED_CORE_FILESYSTEM_HXX

//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core/Common.hxx>

namespace Core
{
    class FileSystem final
    {
    public:
        FileSystem() noexcept = delete;
        FileSystem(const FileSystem&) = delete;
        FileSystem& operator = (const FileSystem&) = delete;

    public:
        static bool Load(std::vector<uint8_t>& result, const std::string& path) noexcept;
    };
}

#endif // INCLUDED_CORE_FILESYSTEM_HXX