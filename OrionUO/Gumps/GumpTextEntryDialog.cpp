﻿// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/***********************************************************************************
**
** GumpTextEntryDialog.cpp
**
** Copyright (C) August 2016 Hotride
**
************************************************************************************
*/

#include "stdafx.h"
#include "GumpTextEntryDialog.h"

CGumpTextEntryDialog::CGumpTextEntryDialog(
    uint serial, short x, short y, uchar variant, int maxLength, string text, string description)
    : CGump(GT_TEXT_ENTRY_DIALOG, serial, x, y)
    , Text(text)
    , m_Description(description)
    , Variant(variant)
    , m_MaxLength(maxLength)
{
    DEBUG_TRACE_FUNCTION;
    NoMove = true;
    Blocked = true;

    if (!g_GrayMenuCount)
    {
        g_Orion.InitScreen(GS_GAME_BLOCKED);
        g_GameBlockedScreen.Code = 2;
    }

    g_GrayMenuCount++;
}

CGumpTextEntryDialog::~CGumpTextEntryDialog()
{
}

void CGumpTextEntryDialog::PrepareContent()
{
    DEBUG_TRACE_FUNCTION;
    if (m_TextField != nullptr && m_Entry != nullptr)
    {
        ushort newGraphic = 0x0475; //Text field

        if (g_EntryPointer == &m_Entry->m_Entry)
            newGraphic = 0x0477; //Text field (active)
        else if (g_SelectedObject.Object == m_Entry)
            newGraphic = 0x0476; //Text field (lighted)

        if (m_TextField->Graphic != newGraphic)
        {
            m_TextField->Graphic = newGraphic;
            WantRedraw = true;
        }
    }
}

void CGumpTextEntryDialog::UpdateContent()
{
    DEBUG_TRACE_FUNCTION;
    Clear();

    Add(new CGUIGumppic(0x0474, 0, 0));

    CGUIText *text = (CGUIText *)Add(new CGUIText(0x0386, 60, 50));
    text->CreateTextureA(2, Text);

    text = (CGUIText *)Add(new CGUIText(0x0386, 60, 108));
    text->CreateTextureA(2, m_Description);

    CGump *gumpEntry = g_GumpManager.GetTextEntryOwner();

    if (gumpEntry != nullptr)
        gumpEntry->WantRedraw = true;

    m_TextField = (CGUIGumppic *)Add(new CGUIGumppic(0x0477, 60, 130));
    m_TextField->Serial = ID_GTED_TEXT_FIELD;

    m_Entry = (CGUITextEntry *)Add(
        new CGUITextEntry(ID_GTED_TEXT_FIELD, 0x0386, 0x0386, 0x0386, 71, 137, 0, false, 1));
    m_Entry->m_Entry.MaxLength = m_MaxLength;
    m_Entry->m_Entry.NumberOnly = (Variant == 2);
    m_Entry->CheckOnSerial = true;
    g_EntryPointer = &m_Entry->m_Entry;

    Add(new CGUIButton(ID_GTED_BUTTON_OKAY, 0x047B, 0x047D, 0x047C, 117, 190));

    if (!NoClose)
        Add(new CGUIButton(ID_GTED_BUTTON_CANCEL, 0x0478, 0x047A, 0x0479, 204, 190));
}

void CGumpTextEntryDialog::GUMP_BUTTON_EVENT_C
{
    DEBUG_TRACE_FUNCTION;
    if (serial == ID_GTED_BUTTON_OKAY) //Button okay
        SendTextEntryDialogResponse(true);
    else if (serial == ID_GTED_BUTTON_CANCEL) //Button cancel
        SendTextEntryDialogResponse(false);
}

#if USE_WISP
void CGumpTextEntryDialog::OnCharPress(const WPARAM &wParam, const LPARAM &lParam)
{
    DEBUG_TRACE_FUNCTION;
    if (Variant == 2) //Только числа
    {
        if (wParam >= '0' && wParam <= '9')
        {
            g_EntryPointer->Insert((wchar_t)wParam);

            int val = atoi(g_EntryPointer->c_str());

            if (val > m_MaxLength)
                g_EntryPointer->Remove(true);
            else
                WantRedraw = true;
        }
    }
    else if (Variant == 1) //Что угодно, но с ограничением по длине
    {
        if ((int)g_EntryPointer->Length() < m_MaxLength)
        {
            g_EntryPointer->Insert((wchar_t)wParam);
            WantRedraw = true;
        }
    }
}
#else
void CGumpTextEntryDialog::OnTextInput(const SDL_TextInputEvent &ev)
{
    NOT_IMPLEMENTED; // FIXME
}
#endif

void CGumpTextEntryDialog::OnKeyDown(const KeyEvent &ev)
{
    DEBUG_TRACE_FUNCTION;

    const auto key = EvKey(ev);
    switch (key)
    {
        case KEY_RETURN:
        case KEY_ESCAPE:
        {
            SendTextEntryDialogResponse(key == KEY_RETURN);
            break;
        }
        case KEY_HOME:
        case KEY_END:
        case KEY_LEFT:
        case KEY_RIGHT:
        case KEY_BACK:
        case KEY_DELETE:
        {
            g_EntryPointer->OnKey(this, key);
            WantRedraw = true;
            break;
        }
        default:
            break;
    }
}

void CGumpTextEntryDialog::SendTextEntryDialogResponse(bool mode)
{
    DEBUG_TRACE_FUNCTION;
    if (!RemoveMark &&
        m_Entry != nullptr) //Непредвиденная ошибка при отсутствии поля ввода текста в гампе
    {
        //Отправляем ответ на ентри диалог
        CPacketTextEntryDialogResponse(this, &m_Entry->m_Entry, mode).Send();
    }

    //Удаляем использованный гамп
    RemoveMark = true;
}

