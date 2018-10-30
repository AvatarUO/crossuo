﻿/***********************************************************************************
**
** SkillsManager.h
**
** Copyright (C) Obtober 2017 Hotride
**
************************************************************************************
*/

#ifndef SKILLSMANAGER_H
#define SKILLSMANAGER_H

class CSkill
{
public:
    bool Button = 0;
    string Name = "";
    float BaseValue = 0.0f;
    float Value = 0.0f;
    float Cap = 0.0f;
    uchar Status = 0;

    CSkill() {}
    CSkill(bool haveButton, const string &name);
    ~CSkill() {}
};

class CSkillsManager
{
public:
    uint Count = 0;
    bool SkillsRequested = false;
    float SkillsTotal = 0.0f;

private:
    vector<CSkill> m_Skills;
    vector<uint8_t> m_SortedTable;

    bool CompareName(const string &str1, const string &str2);

public:
    CSkillsManager() {}
    ~CSkillsManager() {}

    bool Load();

    void Add(const CSkill &skill);

    void Clear();

    CSkill *Get(uint index);

    void Sort();

    int GetSortedIndex(uint index);

    void UpdateSkillsSum();
};

extern CSkillsManager g_SkillsManager;

#endif //SKILLSMANAGER_H
