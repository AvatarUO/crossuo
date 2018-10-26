﻿/***********************************************************************************
**
** PressedObject.h
**
** Copyright (C) August 2016 Hotride
**
************************************************************************************
*/

#ifndef PRESSEDOBJECT_H
#define PRESSEDOBJECT_H

class CPressedObject
{
public:
    uint LeftSerial = 0;
    CRenderObject *LeftObject = nullptr;
    CGump *LeftGump = nullptr;
    uint RightSerial = 0;
    CRenderObject *RightObject = nullptr;
    CGump *RightGump = nullptr;
    uint MidSerial = 0;
    CRenderObject *MidObject = nullptr;
    CGump *MidGump = nullptr;
    CPressedObject();
    virtual ~CPressedObject();

    void ClearAll();

    void Clear(CRenderObject *obj);

    void ClearLeft();
    void ClearRight();
    void ClearMid();

    void Init(const CPressedObject &obj);

    void InitLeft(CRenderObject *obj, CGump *gump = nullptr);
    void InitLeft(const CSelectedObject &obj);

    void InitRight(CRenderObject *obj, CGump *gump = nullptr);
    void InitRight(const CSelectedObject &obj);

    void InitMid(CRenderObject *obj, CGump *gump = nullptr);
    void InitMid(const CSelectedObject &obj);

    bool TestMoveOnDrag();
};

extern CPressedObject g_PressedObject;

#endif
