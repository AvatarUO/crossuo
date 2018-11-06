﻿// MIT License
// Copyright (C) August 2016 Hotride

#include <SDL_timer.h>

COrionWindow g_OrionWindow;

COrionWindow::COrionWindow()
    : Wisp::CWindow()
{
#if 0
    // Add some tests, this cliloc message seems to use broken UTF8
    // "Hey buddy... Looking for work?"
    //            ^^- c2 a0 - is an invalid code point
    uint8_t d[] = {
        0x48, 0x65, 0x79, 0x20, 0x62, 0x75, 0x64, 0x64, 0x79, 0x2E, 0xC2, 
        0xA0, 0x20, 0x4C, 0x6F, 0x6F, 0x6B, 0x69, 0x6E, 0x67, 0x20, 0x66, 
        0x6F, 0x72, 0x20, 0x77, 0x6F, 0x72, 0x6B, 0x3F
    };
    std::string s( reinterpret_cast< char const* >( d ) ) ;
    DecodeUTF8(s);
#endif
}

COrionWindow::~COrionWindow()
{
}

void COrionWindow::SetRenderTimerDelay(int delay)
{
    DEBUG_TRACE_FUNCTION;
    auto timer = GetThreadedTimer(RENDER_TIMER_ID);
    if (timer != nullptr)
        timer->ChangeDelay(delay);
}

bool COrionWindow::OnCreate()
{
    DEBUG_TRACE_FUNCTION;
    if (!g_GL.Install())
    {
        LOG("Error install OpenGL\n");
        ShowMessage("Error install OpenGL", "Error install OpenGL!");
        return false;
    }

    if (!g_Orion.Install())
        return false;

    g_GL.UpdateRect();
    CreateThreadedTimer(RENDER_TIMER_ID, FRAME_DELAY_ACTIVE_WINDOW, false, true, true);
    //CreateThreadedTimer(UPDATE_TIMER_ID, 10);
    CreateTimer(UPDATE_TIMER_ID, 10);
    return true;
}

void COrionWindow::OnDestroy()
{
    DEBUG_TRACE_FUNCTION;
    g_PluginManager.WindowProc(Handle, WM_CLOSE, 0, 0);
    g_Orion.Uninstall();
    Wisp::g_WispCrashLogger.Close();
#if USE_WISP
    ::remove(CStringFromPath(Wisp::g_WispCrashLogger.FileName));
#endif
}

void COrionWindow::OnResize()
{
    DEBUG_TRACE_FUNCTION;
    g_GL.UpdateRect();
}

void COrionWindow::EmulateOnLeftMouseButtonDown()
{
    DEBUG_TRACE_FUNCTION;
    if (g_CurrentScreen != nullptr && g_ScreenEffectManager.Mode == SEM_NONE)
    {
        g_CurrentScreen->SelectObject();
        g_PressedObject.InitLeft(g_SelectedObject);
        if (g_SelectedObject.Object != nullptr || g_GameState == GS_GAME)
        {
            Wisp::g_WispMouse->LeftDropPosition = Wisp::g_WispMouse->Position;
            g_CurrentScreen->OnLeftMouseButtonDown();
        }
    }
}

void COrionWindow::OnLeftMouseButtonDown()
{
    DEBUG_TRACE_FUNCTION;
    if (g_CurrentScreen != nullptr && g_ScreenEffectManager.Mode == SEM_NONE)
    {
        g_GeneratedMouseDown = false;
        g_CurrentScreen->SelectObject();
        g_PressedObject.InitLeft(g_SelectedObject);
        if (g_SelectedObject.Object != nullptr || g_GameState == GS_GAME)
            g_CurrentScreen->OnLeftMouseButtonDown();
    }
}

void COrionWindow::OnLeftMouseButtonUp()
{
    DEBUG_TRACE_FUNCTION;
    if (g_CurrentScreen != nullptr && g_ScreenEffectManager.Mode == SEM_NONE)
    {
        g_CurrentScreen->SelectObject();

        //if ((g_SelectedObject.Object() != nullptr && g_SelectedObject.Object() == g_PressedObject.LeftObject && g_SelectedObject.Serial) || g_GameState >= GS_GAME)
        if ((g_SelectedObject.Object != nullptr && g_SelectedObject.Serial) ||
            g_GameState >= GS_GAME)
        {
            g_CurrentScreen->OnLeftMouseButtonUp();
            if (g_MovingFromMouse && g_PressedObject.LeftGump == nullptr)
                g_AutoMoving = true;
        }

        if (g_PressedObject.LeftObject != nullptr && g_PressedObject.LeftObject->IsGUI() &&
            ((CBaseGUI *)g_PressedObject.LeftObject)->Type == GOT_COMBOBOX &&
            g_PressedObject.LeftGump != nullptr)
        {
            g_PressedObject.LeftGump->WantRedraw = true;
        }

        g_PressedObject.ClearLeft();
    }
}

bool COrionWindow::OnLeftMouseButtonDoubleClick()
{
    DEBUG_TRACE_FUNCTION;
    bool result = false;

    if (g_CurrentScreen != nullptr && g_ScreenEffectManager.Mode == SEM_NONE)
    {
        g_CurrentScreen->SelectObject();
        g_PressedObject.InitLeft(g_SelectedObject);
        result = (g_SelectedObject.Object != nullptr &&
                  g_SelectedObject.Object == g_PressedObject.LeftObject) &&
                 g_CurrentScreen->OnLeftMouseButtonDoubleClick();

        if (result)
        {
            g_PressedObject.ClearLeft();
            g_ClickObject.Clear();
        }
    }

    return result;
}

void COrionWindow::OnRightMouseButtonDown()
{
    DEBUG_TRACE_FUNCTION;
    if (g_CurrentScreen != nullptr && g_ScreenEffectManager.Mode == SEM_NONE)
    {
        g_CurrentScreen->SelectObject();
        g_PressedObject.InitRight(g_SelectedObject);
        g_CurrentScreen->OnRightMouseButtonDown();
        if (g_SelectedObject.Gump == nullptr &&
            !(g_MouseManager.Position.X < g_ConfigManager.GameWindowX ||
              g_MouseManager.Position.Y < g_ConfigManager.GameWindowY ||
              g_MouseManager.Position.X >
                  (g_ConfigManager.GameWindowX + g_ConfigManager.GameWindowWidth) ||
              g_MouseManager.Position.Y >
                  (g_ConfigManager.GameWindowY + g_ConfigManager.GameWindowHeight)))
        {
            g_MovingFromMouse = true;
            g_AutoMoving = false;
        }
    }
}

void COrionWindow::OnRightMouseButtonUp()
{
    DEBUG_TRACE_FUNCTION;
    if (g_CurrentScreen != nullptr && g_ScreenEffectManager.Mode == SEM_NONE)
    {
        g_CurrentScreen->SelectObject();
        if ((g_SelectedObject.Object != nullptr &&
             g_SelectedObject.Object == g_PressedObject.RightObject && g_SelectedObject.Serial) ||
            g_GameState >= GS_GAME)
        {
            g_CurrentScreen->OnRightMouseButtonUp();
        }
        g_MovingFromMouse = false;
        g_PressedObject.ClearRight();
    }
}

bool COrionWindow::OnRightMouseButtonDoubleClick()
{
    DEBUG_TRACE_FUNCTION;
    bool result = false;
    if (g_CurrentScreen != nullptr && g_ScreenEffectManager.Mode == SEM_NONE)
    {
        g_CurrentScreen->SelectObject();
        g_PressedObject.InitRight(g_SelectedObject);
        result = (g_SelectedObject.Object != nullptr &&
                  g_SelectedObject.Object == g_PressedObject.RightObject) &&
                 g_CurrentScreen->OnRightMouseButtonDoubleClick();

        if (result)
        {
            g_PressedObject.ClearRight();
        }
    }

    return result;
}

void COrionWindow::OnMidMouseButtonDown()
{
    DEBUG_TRACE_FUNCTION;
    if (g_PluginManager.WindowProc(Handle, WM_MBUTTONDOWN, 0, 0))
        return;

    if (g_CurrentScreen != nullptr && g_ScreenEffectManager.Mode == SEM_NONE)
        g_CurrentScreen->OnMidMouseButtonDown();
}

void COrionWindow::OnMidMouseButtonUp()
{
    DEBUG_TRACE_FUNCTION;
    if (g_PluginManager.WindowProc(Handle, WM_MBUTTONUP, 0, 0))
        return;

    if (g_CurrentScreen != nullptr && g_ScreenEffectManager.Mode == SEM_NONE)
        g_CurrentScreen->OnMidMouseButtonUp();
}

bool COrionWindow::OnMidMouseButtonDoubleClick()
{
    DEBUG_TRACE_FUNCTION;
    if (g_CurrentScreen != nullptr && g_ScreenEffectManager.Mode == SEM_NONE)
        return g_CurrentScreen->OnMidMouseButtonDoubleClick();

    return false;
}

void COrionWindow::OnMidMouseButtonScroll(bool up)
{
    DEBUG_TRACE_FUNCTION;
    if (g_PluginManager.WindowProc(Handle, WM_MOUSEWHEEL, (up ? 0 : 0x11110000), 0))
        return;

    if (g_CurrentScreen != nullptr && g_ScreenEffectManager.Mode == SEM_NONE)
    {
        g_CurrentScreen->SelectObject();
        g_CurrentScreen->OnMidMouseButtonScroll(up);
    }
}

void COrionWindow::OnXMouseButton(bool up)
{
    DEBUG_TRACE_FUNCTION;
    if (g_PluginManager.WindowProc(Handle, WM_XBUTTONDOWN, (up ? 0 : 0x11110000), 0))
        return;
}

void COrionWindow::OnDragging()
{
    DEBUG_TRACE_FUNCTION;
    if (g_CurrentScreen != nullptr && g_ScreenEffectManager.Mode == SEM_NONE)
        g_CurrentScreen->OnDragging();
}

void COrionWindow::OnActivate()
{
    DEBUG_TRACE_FUNCTION;
    g_Orion.ResumeSound();
    SetRenderTimerDelay(g_FrameDelay[WINDOW_ACTIVE]);
    if (!g_PluginManager.Empty())
        g_PluginManager.WindowProc(Handle, WM_NCACTIVATE, 1, 0);
}

void COrionWindow::OnDeactivate()
{
    DEBUG_TRACE_FUNCTION;
    if (!g_ConfigManager.BackgroundSound)
        g_Orion.PauseSound();

    if (g_ConfigManager.GetReduceFPSUnactiveWindow())
       SetRenderTimerDelay(g_FrameDelay[WINDOW_INACTIVE]);

    if (!g_PluginManager.Empty())
        g_PluginManager.WindowProc(Handle, WM_NCACTIVATE, 0, 0);
}

void COrionWindow::OnTextInput(const TextEvent &ev)
{
    DEBUG_TRACE_FUNCTION;

#if USE_WISP
    if (g_PluginManager.WindowProc(Handle, WM_CHAR, ev.wParam, ev.lParam))
        return;
#endif

    const auto ch = EvChar(ev);
    if ((IsPrintable(ch) || (g_GameState >= GS_GAME && (ch == 0x11 || ch == 0x17))) &&
        g_CurrentScreen != nullptr && g_ScreenEffectManager.Mode == SEM_NONE)
    {
        g_CurrentScreen->OnTextInput(ev);
    }
    else if (ch == KEY_RETURN)
    {
        const auto kev = AsKeyEvent(ev);
        g_CurrentScreen->OnKeyDown(kev);
    }
    else if (ch == 0x16 && g_EntryPointer != nullptr)
    {
        if (g_GameState == GS_MAIN)
            g_MainScreen.Paste();
        else
            g_EntryPointer->Paste();
    }
}

/*
void COrionWindow::OnTextInput(const TextEvent &ev)
{
    DEBUG_TRACE_FUNCTION;
    //if (g_PluginManager.WindowProc(Handle, WM_CHAR, wParam, lParam))
    //	return;

    if (g_CurrentScreen != nullptr && g_ScreenEffectManager.Mode == SEM_NONE)
    {
        g_CurrentScreen->OnTextInput(ev);
    }
}

void COrionWindow::OnKeyDown(const KeyEvent &ev)
{
    const bool isCtrl = (SDL_GetModState() & KMOD_CTRL) != 0;
    if (isCtrl && ev.keysym.sym == SDLK_v && g_EntryPointer != nullptr)
    {
        if (g_GameState == GS_MAIN)
            g_MainScreen.Paste();
        else
            g_EntryPointer->Paste();
    }
    else if (
        g_CurrentScreen != nullptr &&
        g_ScreenEffectManager.Mode == SEM_NONE)
        g_CurrentScreen->OnKeyDown(ev);
}
*/

void COrionWindow::OnKeyDown(const KeyEvent &ev)
{
    DEBUG_TRACE_FUNCTION;

#if USE_WISP
    if (g_PluginManager.WindowProc(Handle, WM_KEYDOWN, ev.wParam, ev.lParam))
        return;
#endif

    const auto key = EvKey(ev);
    if (key != KEY_RETURN && g_CurrentScreen != nullptr && g_ScreenEffectManager.Mode == SEM_NONE)
    {
        g_CurrentScreen->OnKeyDown(ev);
    }
}

void COrionWindow::OnKeyUp(const KeyEvent &ev)
{
    DEBUG_TRACE_FUNCTION;

#if USE_WISP
    if (g_PluginManager.WindowProc(Handle, WM_KEYUP, ev.wParam, ev.lParam))
        return;
#endif

    if (g_CurrentScreen != nullptr && g_ScreenEffectManager.Mode == SEM_NONE)
    {
        g_CurrentScreen->OnKeyUp(ev);
    }

    const auto key = EvKey(ev);
    if (key == KEY_PRINTSCREEN)
    {
        g_ScreenshotBuilder.SaveScreen();
    }
}

HRESULT COrionWindow::OnRepaint(const WPARAM &wParam, const LPARAM &lParam)
{
    DEBUG_TRACE_FUNCTION;
    if (!g_PluginManager.Empty())
        return (HRESULT)g_PluginManager.WindowProc(Handle, WM_NCPAINT, wParam, lParam);

    return (HRESULT)DefWindowProc(Handle, WM_NCPAINT, wParam, lParam);
}

void COrionWindow::OnShow(bool show)
{
    DEBUG_TRACE_FUNCTION;
    if (!g_PluginManager.Empty())
        g_PluginManager.WindowProc(Handle, WM_SHOWWINDOW, (WPARAM)show, 0);
}

void COrionWindow::OnSetText(const LPARAM &lParam)
{
    DEBUG_TRACE_FUNCTION;
    if (!g_PluginManager.Empty())
        g_PluginManager.WindowProc(Handle, WM_SETTEXT, 0, lParam);
}

void COrionWindow::OnTimer(uint32_t id)
{
    DEBUG_TRACE_FUNCTION;
    if (id == UPDATE_TIMER_ID)
    {
        g_Ticks = SDL_GetTicks();
        g_Orion.Process(false);
    }
    if (id == FASTLOGIN_TIMER_ID)
    {
        RemoveTimer(FASTLOGIN_TIMER_ID);
        g_Orion.Connect();
    }
}

void COrionWindow::OnThreadedTimer(uint32_t nowTime, Wisp::CThreadedTimer *timer)
{
    DEBUG_TRACE_FUNCTION;

    g_Ticks = nowTime;
    if (timer->TimerID == RENDER_TIMER_ID)
    {
        g_Orion.Process(true);
    }
    else if (timer->TimerID == UPDATE_TIMER_ID)
    {
        g_Orion.Process(false);
    }
}

bool COrionWindow::OnUserMessages(const UserEvent &ev)
{
    DEBUG_TRACE_FUNCTION;

    switch (ev.code)
    {
        case UOMSG_RECV:
        {
            g_PacketManager.SavePluginReceivePacket((PBYTE)ev.data1, (int)ev.data2);
            return true;
        }
        break;

        case UOMSG_SEND:
        {
            uint32_t ticks = g_Ticks;
            uint8_t *buf = (uint8_t *)ev.data1;
            int size = (int)ev.data2;
            g_TotalSendSize += size;

            CPacketInfo &type = g_PacketManager.GetInfo(*buf);

            LOG("--- ^(%d) s(+%d => %d) Plugin->Server:: %s\n",
                ticks - g_LastPacketTime,
                size,
                g_TotalSendSize,
                type.Name);

            g_LastPacketTime = ticks;
            g_LastSendTime = ticks;

            if (*buf == 0x80 || *buf == 0x91)
            {
                LOG_DUMP(buf, 1);
                SAFE_LOG_DUMP(buf, size);
                LOG("**** ACCOUNT AND PASSWORD CENSORED ****\n");
            }
            else
            {
                LOG_DUMP(buf, size);
            }
            g_ConnectionManager.Send((PBYTE)ev.data1, (int)ev.data2);
            return true;
        }
        break;

        case UOMSG_PATHFINDING:
        {
            const auto xy = (uint32_t)ev.data1;
            const auto zd = (uint32_t)ev.data2;
            return !g_PathFinder.WalkTo(
                (xy >> 16) & 0xFFFF, xy & 0xFFFF, (zd >> 16) & 0xFFFF, zd & 0xFFFF);
        }
        break;

        case UOMSG_WALK:
        {
            const auto run = (bool)ev.data1;
            const auto dir = (uint8_t)ev.data2;
            return !g_PathFinder.Walk(run, dir);
        }
        break;

        case UOMSG_MENU_RESPONSE:
        {
            PUOI_MENU_RESPONSE data = (PUOI_MENU_RESPONSE)ev.data1;
            if (!data->Serial && !data->ID)
            {
                for (CGump *gump = (CGump *)g_GumpManager.m_Items; gump != nullptr;)
                {
                    CGump *next = (CGump *)gump->m_Next;
                    if (gump->GumpType == GT_MENU || gump->GumpType == GT_GRAY_MENU)
                    {
                        CPacketMenuResponse(gump, data->Code).Send();
                        g_GumpManager.RemoveGump(gump);
                    }
                    gump = next;
                }
                break;
            }

            CGump *gump = g_GumpManager.GetGump(data->Serial, data->ID, GT_MENU);
            if (gump == nullptr)
            {
                gump = g_GumpManager.GetGump(data->Serial, data->ID, GT_GRAY_MENU);
                if (gump != nullptr)
                {
                    CPacketGrayMenuResponse(gump, data->Code).Send();
                    g_GumpManager.RemoveGump(gump);
                }
            }
            else
            {
                CPacketMenuResponse(gump, data->Code).Send();
                g_GumpManager.RemoveGump(gump);
            }
        }
        break;

        case CPingThread::MessageID:
        {
            PING_INFO_DATA *info = (PING_INFO_DATA *)ev.data1;
            if (info != nullptr)
            {
                if (info->ServerID == 0xFFFFFFFF)
                {
                    memcpy(&g_GameServerPingInfo, info, sizeof(g_GameServerPingInfo));
                    char ping[50] = { 0 };
                    sprintf_s(
                        ping,
                        "ping(min:%i max:%i avg:%i lost:%i) ",
                        g_GameServerPingInfo.Min,
                        g_GameServerPingInfo.Max,
                        g_GameServerPingInfo.Average,
                        g_GameServerPingInfo.Lost);
                    g_PingString = ping;
                }
                else
                {
                    CServer *server = g_ServerList.GetServer(info->ServerID);

                    if (server != nullptr)
                    {
                        if (info->Min < 9999)
                        {
                            server->Ping = info->Average;
                            server->PacketsLoss = info->Lost;
                            g_ServerScreen.UpdateContent();
                        }
                    }
                }

                LOG("Ping info: id:%i min:%i max:%i average:%i lost:%i\n",
                    info->ServerID,
                    info->Min,
                    info->Max,
                    info->Average,
                    info->Lost);
            }
        }
        break;

        case Wisp::CThreadedTimer::MessageID:
        {
            auto nowTime = (uint32_t)ev.data1;
            auto timer = (Wisp::CThreadedTimer *)ev.data2;
            OnThreadedTimer(nowTime, timer);
            //DebugMsg("OnThreadedTimer %i, 0x%08X\n", nowTime, timer);
        }
        break;

        case COrionWindow::MessageID:
        {
            OnTimer((uint32_t)ev.data1);
            break;
        }

        default:
        {
        }
        break;
    }

#if USE_WISP
    g_PluginManager.WindowProc(Handle, ev.code, ev.data1, ev.data2);
#else
    // NOT_IMPLEMENTED;
#endif
    return true;
}
