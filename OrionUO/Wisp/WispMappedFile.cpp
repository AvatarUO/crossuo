
#include "FileSystem.h"

namespace Wisp
{
CMappedFile::CMappedFile()

{
}

CMappedFile::~CMappedFile()
{
    Unload();
}

#if USE_WISP
bool CMappedFile::Load()
{
    DEBUG_TRACE_FUNCTION;
    bool result = false;
    Size = GetFileSize(m_File, nullptr);
    if (Size > 0)
    {
        m_Map = CreateFileMapping(m_File, nullptr, 2, 0, 0, nullptr);

        if (m_Map != nullptr)
        {
            Start = (uint8_t *)MapViewOfFile(m_Map, FILE_MAP_READ, 0, 0, Size);

            result = (Start != nullptr);

            if (!result)
            {
                CloseHandle(m_Map);
                CloseHandle(m_File);
                m_Map = nullptr;
                m_File = INVALID_HANDLE_VALUE;
            }
            else
                SetData(Start, Size);
        }
        else
        {
            CloseHandle(m_File);
            m_File = INVALID_HANDLE_VALUE;
        }
    }
    else
    {
        CloseHandle(m_File);
        m_File = INVALID_HANDLE_VALUE;
    }

    return result;
}
#endif

bool CMappedFile::Load(const os_path &path)
{
    DEBUG_TRACE_FUNCTION;
    LOG("Mmaping  %s\n", CStringFromPath(path));
    bool result = false;

    if (fs_path_exists(path))
    {
        Unload();

#if USE_WISP
        m_File = CreateFileW(
            path.c_str(), GENERIC_READ, 1, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (m_File != INVALID_HANDLE_VALUE)
            result = Load();
        else
            LOG("INVALID_HANDLE_VALUE for CreateFileW  %s\n", CStringFromPath(path));
#else
        Start = fs_map(path, &Size);
        result = Start != nullptr;
        SetData(Start, Size);
#endif
    }
    else
    {
        LOG("File not found %s\n", CStringFromPath(path));
    }

    if (!result)
    {
#if USE_WISP
        auto errorCode = GetLastError();
#else
        auto errorCode = errno;
#endif
        LOG("Failed to memory map, error code: %i\n", errorCode);
    }

    return result;
}

void CMappedFile::Unload()
{
    //DEBUG_TRACE_FUNCTION;
#if USE_WISP
    if (Start != nullptr)
        UnmapViewOfFile(Start);

    if (m_Map != nullptr)
    {
        CloseHandle(m_Map);
        m_Map = 0;
    }

    if (m_File != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_File);
        m_File = INVALID_HANDLE_VALUE;
    }
#else
    if (Start != nullptr)
    {
        fs_unmap(Start, Size);
    }
#endif

    SetData(nullptr, 0);
}

}; // namespace Wisp
