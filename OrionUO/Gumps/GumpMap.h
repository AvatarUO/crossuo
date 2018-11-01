﻿/***********************************************************************************
**
** GumpMap.h
**
** Copyright (C) August 2016 Hotride
**
************************************************************************************
*/

#ifndef GUMPMAP_H
#define GUMPMAP_H

class CGumpMap : public CGump
{
protected:
    int m_PlotState = 0;

public:
    int GetPlotState() { return m_PlotState; };
    void SetPlotState(int val);
    int StartX = 0;
    int StartY = 0;
    int EndX = 0;
    int EndY = 0;
    int Width = 0;
    int Height = 0;

private:
    static const int ID_GM_PLOT_COURSE = 1;
    static const int ID_GM_STOP_PLOTTING = 2;
    static const int ID_GM_CLEAR_COURSE = 3;
    static const int ID_GM_PIN_LIST = 10;
    static const int ID_GM_PIN_LIST_INSERT = 1000;

    uint32_t m_PinTimer{ 0 };

    CBaseGUI *m_PinOnCursor{ nullptr };
    CBaseGUI *m_FakeInsertionPin{ nullptr };

    CGUIButton *m_PlotCourse{ nullptr };
    CGUIButton *m_StopPlotting{ nullptr };
    CGUIButton *m_ClearCourse{ nullptr };

    vector<CGUIText *> m_Labels;

    int LineUnderMouse(int &x1, int &y1, int x2, int y2);

public:
    CGumpMap(
        uint32_t serial,
        uint16_t graphic,
        int startX,
        int startY,
        int endX,
        int endY,
        int width,
        int height);
    virtual ~CGumpMap();

    CGUIExternalTexture *m_Texture{ nullptr };
    CGUIDataBox *m_DataBox{ nullptr };

    virtual void PrepareContent();

    virtual void GenerateFrame(bool stop);

    virtual CRenderObject *Select();

    GUMP_BUTTON_EVENT_H;

    void OnLeftMouseButtonDown();
    void OnLeftMouseButtonUp();
};

#endif
