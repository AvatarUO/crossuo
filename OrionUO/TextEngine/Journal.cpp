﻿// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/***********************************************************************************
**
** Journal.cpp
**
** Copyright (C) August 2016 Hotride
**
************************************************************************************
*/

#include "stdafx.h"

CJournal g_Journal(150);

CJournal::CJournal(int maxSize)
    : CBaseQueue()
    , MaxSize(maxSize)
{
}

CJournal::~CJournal()
{
}

void CJournal::Clear()
{
    DEBUG_TRACE_FUNCTION;
    CBaseQueue::Clear();

    Size = 0;
}

void CJournal::Add(CTextData *obj)
{
    DEBUG_TRACE_FUNCTION;
    CBaseQueue::Add(obj);

    CGumpJournal *gump = (CGumpJournal *)g_GumpManager.UpdateGump(0, 0, GT_JOURNAL);

    if (gump != nullptr)
        gump->AddText(obj);

    if (Size >= MaxSize)
    {
        CTextData *item = (CTextData *)m_Items;

        if (gump != nullptr)
            gump->DeleteText(item);

        Delete(item);
    }
    else
        Size++;
}

