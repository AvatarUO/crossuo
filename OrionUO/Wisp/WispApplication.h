// MIT License

#pragma once

#include "FileSystem.h"

namespace Wisp
{
class CApplication
{
public:
    os_path m_ExePath;
    os_path m_UOPath;

protected:
    virtual void OnMainLoop() {}

public:
    CApplication();
    virtual ~CApplication();

#if USE_WISP
    HINSTANCE Hinstance = 0;
    int Run(HINSTANCE hinstance);
#else
    int Run();
#endif

    string GetFileVersion(uint32_t *numericVerion = nullptr) const;
    os_path ExeFilePath(const char *str, ...) const;
    os_path UOFilesPath(const char *str, ...) const;
    os_path UOFilesPath(const string &str, ...) const;
};

extern CApplication *g_WispApplication;

}; // namespace Wisp