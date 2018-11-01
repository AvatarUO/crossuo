﻿// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "stdafx.h"
#include "FileSystem.h"
#include "WispThread.h"
#include <SDL_timer.h>
namespace Wisp
{
CApplication *g_WispApplication = nullptr;

CApplication::CApplication()
{
    LOG("INITIATING CAPPLICATION\n");
    g_MainThread = CThread::GetCurrentThreadId();
    DEBUG_TRACE_FUNCTION;
    g_WispApplication = this;
    m_UOPath = m_ExePath = fs_path_current();
    g_MainScreen.LoadCustomPath();
}

CApplication::~CApplication()
{
    DEBUG_TRACE_FUNCTION;
    g_WispApplication = nullptr;
    Hinstance = 0;
}

int CApplication::Run(HINSTANCE hinstance)
{
    // DEBUG_TRACE_FUNCTION;
#if USE_WISP
    timeBeginPeriod(1);
    Hinstance = hinstance;

    MSG msg = { 0 };

    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
            SDL_Delay(1);

        OnMainLoop();
    }

    timeEndPeriod(1);

    return (int)msg.wParam;
#else
    bool quit = false;
    while (!quit)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            quit = Wisp::g_WispWindow->OnWindowProc(event);
            if (quit)
                break;

            OnMainLoop();
        }
    }

    return EXIT_SUCCESS;
#endif
}

string CApplication::GetFileVersion(uint32_t *numericVerion) const
{
#if USE_WISP
    //File version info
    wchar_t szFilename[MAX_PATH] = { 0 };

    if (GetModuleFileName(Hinstance, &szFilename[0], sizeof(szFilename)))
    {
        DWORD dummy = 0;
        DWORD dwSize = GetFileVersionInfoSize(&szFilename[0], &dummy);

        if (dwSize > 0)
        {
            vector<uint8_t> lpVersionInfo(dwSize, 0);

            if (GetFileVersionInfoW(&szFilename[0], 0, dwSize, &lpVersionInfo[0]))
            {
                UINT uLen = 0;
                VS_FIXEDFILEINFO *lpFfi = nullptr;

                VerQueryValueW(&lpVersionInfo[0], PATH_SEP, (LPVOID *)&lpFfi, &uLen);

                DWORD dwFileVersionMS = 0;
                DWORD dwFileVersionLS = 0;

                if (lpFfi != nullptr)
                {
                    dwFileVersionMS = lpFfi->dwFileVersionMS;
                    dwFileVersionLS = lpFfi->dwFileVersionLS;
                }

                int dwLeftMost = (int)HIWORD(dwFileVersionMS);
                int dwSecondLeft = (int)LOWORD(dwFileVersionMS);
                int dwSecondRight = (int)HIWORD(dwFileVersionLS);
                int dwRightMost = (int)LOWORD(dwFileVersionLS);

                if (numericVerion != nullptr)
                    *numericVerion = ((dwLeftMost & 0xFF) << 24) | ((dwSecondLeft & 0xFF) << 16) |
                                     ((dwSecondRight & 0xFF) << 8) | (dwRightMost & 0xFF);

                char fileVersion[100] = { 0 };
                sprintf_s(
                    fileVersion,
                    "%i.%i.%i.%i",
                    dwLeftMost,
                    dwSecondLeft,
                    dwSecondRight,
                    dwRightMost);

                return fileVersion;
            }
        }
    }

    return "unknown";
#else
    return " Linux";
#endif
}

os_path CApplication::ExeFilePath(const char *str, ...) const
{
    DEBUG_TRACE_FUNCTION;
    va_list arg;
    va_start(arg, str);

    char out[MAX_PATH] = { 0 };
    vsprintf_s(out, str, arg);

    va_end(arg);

    os_path res{ m_ExePath.c_str() };
    res.append(PATH_SEP);
    res.append(ToPath(out));
    return res;
}

os_path CApplication::UOFilesPath(const string &str, ...) const
{
    return UOFilesPath(str.c_str());
}

os_path CApplication::UOFilesPath(const char *str, ...) const
{
    DEBUG_TRACE_FUNCTION;
    va_list arg;
    va_start(arg, str);

    char out[MAX_PATH] = { 0 };
    vsprintf_s(out, str, arg);

    va_end(arg);

    os_path res{ m_UOPath.c_str() };
    res.append(PATH_SEP);
    res.append(ToPath(out));
    return res;
}

}; // namespace Wisp
