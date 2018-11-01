﻿#pragma once
#include "Input.h"

class CMainScreen : public CBaseScreen
{
private:
    enum
    {
        MSCC_ACTID = 1,
        MSCC_ACTPWD,
        MSCC_REMEMBERPWD,
        MSCC_AUTOLOGIN,
        MSCC_SMOOTHMONITOR,
        MSCC_THE_ABYSS,
        MSCC_ASMUT,
        MSCC_CUSTOM_PATH,
        MSCC_COUNT,
    };

    int GetConfigKeyCode(const string &key);

    CGumpScreenMain m_MainGump;

public:
    CMainScreen();
    virtual ~CMainScreen();

    static const uint8_t ID_SMOOTH_MS_QUIT = 1;
    static const uint8_t ID_SMOOTH_MS_CONNECT = 2;

    CEntryText *m_Account = nullptr;
    CEntryText *m_Password = nullptr;
    CGUICheckbox *m_SavePassword = nullptr;
    CGUICheckbox *m_AutoLogin = nullptr;

    void SetAccounting(const string &account, const string &password);

    void Paste();

    void ProcessSmoothAction(uint8_t action = 0xFF);

    void LoadGlobalConfig();

    void LoadCustomPath();

    void SaveGlobalConfig();

    void Init();

#if USE_WISP
    virtual void OnCharPress(const WPARAM &wParam, const LPARAM &lParam);
#else
    virtual void OnTextInput(const SDL_TextInputEvent &ev) override;
#endif
    virtual void OnKeyDown(const KeyEvent &ev) override;
};

extern CMainScreen g_MainScreen;
