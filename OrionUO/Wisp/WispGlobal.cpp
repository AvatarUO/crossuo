﻿// MIT License

#include <locale>
#include <codecvt>
#include <SDL_thread.h>

#include "WispThread.h"

SDL_threadID g_MainThread;
deque<string> g_WispDebugFunStack;
#if USE_WISP_DEBUG_FUNCTION_NAMES == 2
char *g_WispCurrentFunctionName = nullptr;
#endif

CWispFunDebug::CWispFunDebug(const char *str)
{
    if (g_MainThread == Wisp::CThread::GetCurrentThreadId())
        g_WispDebugFunStack.push_back(str);
}

CWispFunDebug::~CWispFunDebug()
{
    if (g_MainThread == Wisp::CThread::GetCurrentThreadId())
        g_WispDebugFunStack.pop_back();
}

int CalculatePercents(int max, int current, int maxValue)
{
    if (max > 0)
    {
        max = (current * 100) / max;

        if (max > 100)
            max = 100;

        if (max > 1)
            max = (maxValue * max) / 100;
    }

    return max;
}

string EncodeUTF8(const wstring &wstr)
{
#if USE_WISP || defined(ORION_WINDOWS)
    int size =
        ::WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), nullptr, 0, nullptr, nullptr);
    string result = "";

    if (size > 0)
    {
        result.resize(size + 1);
        ::WideCharToMultiByte(
            CP_UTF8, 0, &wstr[0], (int)wstr.size(), &result[0], size, nullptr, nullptr);
        result.resize(size); // result[size] = 0;
    }
#else
    mbstate_t state{};
    string result{};
    auto p = wstr.data();
    const auto size = wcsrtombs(nullptr, &p, 0, &state);
    if (size > 0)
    {
        result.resize(size + 1);
        wcsrtombs(&result[0], &p, size, &state);
        result.resize(size);
    }
#endif

    return result;
}

wstring DecodeUTF8(const string &str)
{
#if USE_WISP || defined(ORION_WINDOWS)
    int size = ::MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), nullptr, 0);
    wstring result = {};
    if (size > 0)
    {
        result.resize(size + 1);
        ::MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &result[0], size);
        result.resize(size); // result[size] = 0;
    }
#else
    LOG("\nDecodeUTF8: %s\n\n", str.c_str());
    LOG_DUMP((uint8_t *)str.data(), str.size());
    mbstate_t state{};
    wstring result{};
    auto p = str.data();

    const size_t size = mbsrtowcs(nullptr, &p, 0, &state);
    if (size == -1)
    {
        LOG("\nDecodeUTF8 Failed: %s\n\n", str.c_str());
        LOG_DUMP((uint8_t *)str.data(), str.size());
        return L"Invalid UTF8 sequence found";
    }    
    
    if (size > 0)
    {
        result.resize(size);
        mbsrtowcs(&result[0], &p, size, &state);
    }
#endif
    return result;
}

string ToCamelCaseA(string str)
{
    int offset = 'a' - 'A';
    bool lastSpace = true;

    for (char &c : str)
    {
        if (c == ' ')
            lastSpace = true;
        else if (lastSpace)
        {
            lastSpace = false;

            if (c >= 'a' && c <= 'z')
                c -= offset;
        }
    }

    return str;
}

wstring ToCamelCaseW(wstring str)
{
    int offset = L'a' - L'A';
    bool lastSpace = true;

    for (wchar_t &c : str)
    {
        if (c == L' ')
            lastSpace = true;
        else if (lastSpace)
        {
            lastSpace = false;

            if (c >= L'a' && c <= L'z')
                c -= offset;
        }
    }

    return str;
}

#if defined(ORION_LINUX)
const string &ToString(const string &str)
{
    return str;
}
#endif

string ToString(const wstring &wstr)
{
#if USE_WISP
    string str = "";
    int size = (int)wstr.length();
    int newSize =
        ::WideCharToMultiByte(GetACP(), 0, wstr.c_str(), size, nullptr, 0, nullptr, nullptr);

    if (newSize > 0)
    {
        str.resize(newSize + 1);
        ::WideCharToMultiByte(GetACP(), 0, wstr.c_str(), size, &str[0], newSize, nullptr, nullptr);
        str.resize(newSize); // str[newSize] = 0;
    }
    return str;
#else
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.to_bytes(wstr);
#endif
}

wstring ToWString(const string &str)
{
#if USE_WISP
    int size = (int)str.length();
    wstring wstr = {};

    if (size > 0)
    {
        wstr.resize(size + 1);
        MultiByteToWideChar(GetACP(), 0, str.c_str(), size, &wstr[0], size);
        wstr.resize(size); // wstr[size] = 0;
    }
    return wstr;
#else
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);
#endif
}

string Trim(const string &str)
{
    string::const_iterator it = str.begin();
    for (; it != str.end() && isspace(*it); ++it)
        ;

    string::const_reverse_iterator rit = str.rbegin();
    for (; rit.base() != it && isspace(*rit); ++rit)
        ;

    return string(it, rit.base());
}

int ToInt(const string &str)
{
    return atoi(str.c_str());
}

string ToLowerA(string s)
{
#if defined(ORION_WINDOWS)
    if (s.length())
        _strlwr(&s[0]);

    return s;
#else
    std::transform(s.begin(), s.end(), s.begin(), [](auto c) { return std::tolower(c); });
    return s;
#endif
}

wstring ToLowerW(wstring s)
{
#if defined(ORION_WINDOWS)
    if (s.length())
        _wcslwr(&s[0]);

    return s;
#else
    std::transform(s.begin(), s.end(), s.begin(), [](auto c) { return std::towlower(c); });
    return s;
#endif
}

string ToUpperA(string s)
{
#if defined(ORION_WINDOWS)
    if (s.length())
        _strupr(&s[0]);

    return s;
#else
    std::transform(s.begin(), s.end(), s.begin(), [](auto c) { return std::toupper(c); });
    return s;
#endif
}

wstring ToUpperW(wstring s)
{
#if defined(ORION_WINDOWS)
    if (s.length())
        _wcsupr(&s[0]);

    return s;
#else
    std::transform(s.begin(), s.end(), s.begin(), [](auto c) { return std::towupper(c); });
    return s;
#endif
}

bool Int32TryParse(string str, int &result)
{
    std::istringstream convert(str);
    try
    {
        convert >> result;
    }
    catch (int)
    {
        result = 0;
        return false;
    }
    if (!convert.eof())
    {
        result = 0;
        return false;
    }
    return true;
}

bool ToBool(const string &str)
{
    string data = ToLowerA(str);

    const int countOfTrue = 3;
    const string m_TrueValues[countOfTrue] = { "on", "yes", "true" };
    bool result = false;

    for (int i = 0; i < countOfTrue && !result; i++)
        result = (data == m_TrueValues[i]);

    return result;
}

#if DEBUGGING_OUTPUT == 1
void DebugMsg(const char *format, ...)
{
    va_list arg;
    va_start(arg, format);

    char buf[512] = { 0 };
    vsprintf_s(buf, format, arg);

#if defined(ORION_WINDOWS)
    OutputDebugStringA(buf);
#else
    fprintf(stdout, "%s", buf);
#endif

    va_end(arg);
}

void DebugMsg(const wchar_t *format, ...)
{
    va_list arg;
    va_start(arg, format);

    wchar_t buf[512] = { 0 };
    vswprintf_s(buf, format, arg);

#if USE_WISP
    OutputDebugStringW(buf);
#else
    fprintf(stdout, "%ws", buf);
#endif

    va_end(arg);
}

void DebugDump(uint8_t *data, int size)
{
    int num_lines = size / 16;

    if (size % 16 != 0)
        num_lines++;

    for (int line = 0; line < num_lines; line++)
    {
        int row = 0;

        DebugMsg("%04x: ", line * 16);

        for (row = 0; row < 16; row++)
        {
            if (line * 16 + row < size)
                DebugMsg("%02x ", data[line * 16 + row]);
            else
                DebugMsg("-- ");
        }

        DebugMsg(": ");

        char buf[17] = { 0 };

        for (row = 0; row < 16; row++)
        {
            if (line * 16 + row < size)
                buf[row] = (isprint(data[line * 16 + row]) ? data[line * 16 + row] : '.');
        }

        DebugMsg(buf);
        DebugMsg("\n");
    }
}
#endif
