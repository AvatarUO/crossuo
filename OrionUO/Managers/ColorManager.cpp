﻿// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/***********************************************************************************
**
** ColorManager.cpp
**
** Copyright (C) August 2016 Hotride
**
************************************************************************************
*/

#include "stdafx.h"

CColorManager g_ColorManager;

CColorManager::CColorManager()
    : m_HuesCount(0)
{
}

CColorManager::~CColorManager()
{
}

void CColorManager::Init()
{
    DEBUG_TRACE_FUNCTION;
    intptr_t addr = (intptr_t)g_FileManager.m_HuesMul.Start;
    size_t size = g_FileManager.m_HuesMul.Size;

    if (addr > 0 && size > 0 && addr != -1 && size != -1)
    {
        size_t entryCount = size / sizeof(HUES_GROUP);

        m_HuesCount = (int)entryCount * 8;
        m_HuesRange.resize(entryCount);

        memcpy(&m_HuesRange[0], (void *)addr, entryCount * sizeof(HUES_GROUP));
    }
    else
        m_HuesCount = 0;

    if (g_FileManager.m_RadarcolMul.Size)
    {
        m_Radarcol.resize(g_FileManager.m_RadarcolMul.Size / 2);
        memcpy(
            &m_Radarcol[0],
            (void *)g_FileManager.m_RadarcolMul.Start,
            g_FileManager.m_RadarcolMul.Size);
    }
}

/*!
Патч блока цветов из вердаты
@param [__in] index Индекс в списке
@param [__in] group Указатель на цвета патча
@return 
*/
void CColorManager::SetHuesBlock(int index, PVERDATA_HUES_GROUP group)
{
    DEBUG_TRACE_FUNCTION;
    if (index < 0 || index >= m_HuesCount)
        return;

    m_HuesRange[index].Header = group->Header;
    for (int i = 0; i < 8; i++)
        memcpy(
            &m_HuesRange[index].Entries[i].ColorTable[0],
            &group->Entries[i].ColorTable[0],
            sizeof(uint16_t[32]));
}

/*!
Создание палитры цветов для шейдера
@return
*/
void CColorManager::CreateHuesPalette()
{
    DEBUG_TRACE_FUNCTION;
    m_HuesFloat.resize(m_HuesCount);
    int entryCount = m_HuesCount / 8;

    for (int i = 0; i < entryCount; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            FLOAT_HUES &fh = m_HuesFloat[(i * 8) + j];

            for (int h = 0; h < 32; h++)
            {
                int idx = (int)h * 3;

                uint16_t c = m_HuesRange[i].Entries[j].ColorTable[h];

                fh.Palette[idx] = (((c >> 10) & 0x1F) / 31.0f);
                fh.Palette[idx + 1] = (((c >> 5) & 0x1F) / 31.0f);
                fh.Palette[idx + 2] = ((c & 0x1F) / 31.0f);
            }
        }
    }
}

/*!
Отправка цветов в шейдер
@param [__in] color Индекс цвета
@return
*/
void CColorManager::SendColorsToShader(uint16_t color)
{
    DEBUG_TRACE_FUNCTION;
    if (color != 0)
    {
        if (color & SPECTRAL_COLOR_FLAG)
            glUniform1fv(ShaderColorTable, 32 * 3, &m_HuesFloat[0].Palette[0]);
        else
        {
            if (color >= m_HuesCount)
            {
                color %= m_HuesCount;

                if (!color)
                    color = 1;
            }

            glUniform1fv(ShaderColorTable, 32 * 3, &m_HuesFloat[color - 1].Palette[0]);
        }
    }
}

/*!
Конвертирование цвета из 16 бит в 32 бит
@param [__in] c 16-битный цвет
@return 32-битный цвет
*/
uint32_t CColorManager::Color16To32(uint16_t c)
{
    const uint8_t table[32] = { 0x00, 0x08, 0x10, 0x18, 0x20, 0x29, 0x31, 0x39, 0x41, 0x4A, 0x52,
                              0x5A, 0x62, 0x6A, 0x73, 0x7B, 0x83, 0x8B, 0x94, 0x9C, 0xA4, 0xAC,
                              0xB4, 0xBD, 0xC5, 0xCD, 0xD5, 0xDE, 0xE6, 0xEE, 0xF6, 0xFF };

    return (table[(c >> 10) & 0x1F] | (table[(c >> 5) & 0x1F] << 8) | (table[c & 0x1F] << 16));

    /*return
	(
		(((c >> 10) & 0x1F) * 0xFF / 0x1F) |
		((((c >> 5) & 0x1F) * 0xFF / 0x1F) << 8) |
		(((c & 0x1F) * 0xFF / 0x1F) << 16)
	);*/
}

/*!
Конвертирование цвета из 32 бит в 16 бит
@param [__in] c 32-битный цвет
@return 16-битный цвет
*/
uint16_t CColorManager::Color32To16(int c)
{
    return (((c & 0xFF) * 32) / 256) | (((((c >> 16) & 0xff) * 32) / 256) << 10) |
           (((((c >> 8) & 0xff) * 32) / 256) << 5);
}

/*!
Перевод в серый
@param [__in] c 16-битный цвет
@return 16-битный цвет
*/
uint16_t CColorManager::ConvertToGray(uint16_t c)
{
    return ((c & 0x1F) * 299 + ((c >> 5) & 0x1F) * 587 + ((c >> 10) & 0x1F) * 114) / 1000;
}

/*!
Получить 16-битный цвет
@param [__in] c Исходный цвет
@param [__in] color Индекс цвета в палитре
@return 16-битный цвет
*/
uint16_t CColorManager::GetColor16(uint16_t c, uint16_t color)
{
    DEBUG_TRACE_FUNCTION;
    if (color != 0 && color < m_HuesCount)
    {
        color -= 1;
        int g = color / 8;
        int e = color % 8;

        return m_HuesRange[g].Entries[e].ColorTable[(c >> 10) & 0x1F];
    }

    return c;
}

/*!
Получить 16-битный цвет для радара
@param [__in] c Исходный 16-битный цвет
@return 16-битный цвет
*/
uint16_t CColorManager::GetRadarColorData(int c)
{
    DEBUG_TRACE_FUNCTION;
    if (c < (int)m_Radarcol.size())
        return m_Radarcol[c];

    return 0;
}

/*!
Получить 32-битный цвет без конвертирования входящего цвета
@param [__in] c Исходный 16-битный цвет
@param [__in] color Индекс цвета в палитре
@return 32-битный цвет
*/
uint32_t CColorManager::GetPolygoneColor(uint16_t c, uint16_t color)
{
    DEBUG_TRACE_FUNCTION;
    if (color != 0 && color < m_HuesCount)
    {
        color -= 1;
        int g = color / 8;
        int e = color % 8;

        return Color16To32(m_HuesRange[g].Entries[e].ColorTable[c]);
    }

    return 0xFF010101; //Black
}

/*!
Получить 32-битный цвет для Unicode-шрифтов
@param [__in] c Исходный 16-битный цвет
@param [__in] color Индекс цвета в палитре
@return 32-битный цвет
*/
uint32_t CColorManager::GetUnicodeFontColor(uint16_t &c, uint16_t color)
{
    DEBUG_TRACE_FUNCTION;
    if (color != 0 && color < m_HuesCount)
    {
        color -= 1;
        int g = color / 8;
        int e = color % 8;

        return Color16To32(m_HuesRange[g].Entries[e].ColorTable[8]);
    }

    return Color16To32(c);
}

/*!
Получить 32-битный цвет
@param [__in] c Исходный 16-битный цвет
@param [__in] color Индекс цвета в палитре
@return 32-битный цвет
*/
uint32_t CColorManager::GetColor(uint16_t &c, uint16_t color)
{
    DEBUG_TRACE_FUNCTION;
    if (color != 0 && color < m_HuesCount)
    {
        color -= 1;
        int g = color / 8;
        int e = color % 8;

        return Color16To32(m_HuesRange[g].Entries[e].ColorTable[(c >> 10) & 0x1F]);
    }

    return Color16To32(c);
}

/*!
Получить 32-битный цвет с учетом оттенков серого
@param [__in] c Исходный 16-битный цвет
@param [__in] color Индекс цвета в палитре
@return 32-битный цвет
*/
uint32_t CColorManager::GetPartialHueColor(uint16_t &c, uint16_t color)
{
    DEBUG_TRACE_FUNCTION;
    if (color != 0 && color < m_HuesCount)
    {
        color -= 1;
        int g = color / 8;
        int e = color % 8;

        uint32_t cl = Color16To32(c);

        if (ToColorR(cl) == ToColorG(cl) && ToColorB(cl) == ToColorG(cl))
            return Color16To32(m_HuesRange[g].Entries[e].ColorTable[(c >> 10) & 0x1F]);

        return cl;
    }

    return Color16To32(c);
}

uint16_t CColorManager::FixColor(uint16_t color, uint16_t defaultColor)
{
    uint16_t fixedColor = color & 0x3FFF;

    if (fixedColor)
    {
        if (fixedColor >= 0x0BB8)
            fixedColor = 1;

        fixedColor |= (color & 0xC000);
    }
    else
    {
        fixedColor = defaultColor;
    }

    return fixedColor;
}
