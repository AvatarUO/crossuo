﻿// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/***********************************************************************************
**
** GumpDye.h
**
** Copyright (C) August 2016 Hotride
**
************************************************************************************
*/

#include "stdafx.h"

CGumpDye::CGumpDye(uint serial, short x, short y, ushort graphic)
    : CGumpSelectColor(serial, x, y, SCGS_OPT_TOOLTIP_TEXT)
{
    NoClose = true;
    GumpType = GT_DYE;
    Graphic = graphic;
}

CGumpDye::~CGumpDye()
{
}

ushort CGumpDye::GetCurrentColor()
{
    DEBUG_TRACE_FUNCTION;
    ushort startColor = m_ColorRef + 2;
    ushort color = 0;

    for (int y = 0; y < 10 && !color; y++)
    {
        for (int x = 0; x < 20; x++)
        {
            if (m_SelectedIndex == ID_GSC_COLORS + (x * 30 + y))
            {
                color = startColor;
                break;
            }

            startColor += 5;
        }
    }

    return color;
}

void CGumpDye::UpdateContent()
{
    DEBUG_TRACE_FUNCTION;
    CGumpSelectColor::UpdateContent();

    if (m_Tube == nullptr)
    {
        Add(new CGUIShader(&g_ColorizerShader, true));

        m_Tube = (CGUITilepic *)Add(new CGUITilepic(0x0FAB, GetCurrentColor(), 200, 58));
        m_Tube->PartialHue = true;

        Add(new CGUIShader(&g_ColorizerShader, false));
    }
    else
        m_Tube->Color = GetCurrentColor();
}

void CGumpDye::OnSelectColor(ushort color)
{
    DEBUG_TRACE_FUNCTION;
    CPacketDyeDataResponse(Serial, Graphic, color + 1).Send();
    RemoveMark = true;
}

void CGumpDye::GUMP_BUTTON_EVENT_C
{
    DEBUG_TRACE_FUNCTION;
    CGumpSelectColor::OnButton(serial);

    if (serial >= ID_GSC_COLORS && m_Tube != nullptr)
        m_Tube->Color = GetCurrentColor();
}

void CGumpDye::GUMP_SLIDER_CLICK_EVENT_C
{
    DEBUG_TRACE_FUNCTION;
    OnSliderMove(serial);
}

void CGumpDye::GUMP_SLIDER_MOVE_EVENT_C
{
    DEBUG_TRACE_FUNCTION;
    CGumpSelectColor::OnSliderMove(serial);

    if (m_Tube != nullptr)
        m_Tube->Color = GetCurrentColor();
}

