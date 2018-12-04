#include "Stubs.h"

#if !defined(ORION_WINDOWS)

#include <cassert>
#include <cwctype>
#include <errno.h>

// Bad and very ugly "API" stuff
uint32_t DefWindowProc(void * /*unused*/, uint32_t /*unused*/, void * /*unused*/, void * /*unused*/)
{
    ORION_NOT_IMPLEMENTED;
    return 0;
}
int GetSystemDefaultLangID()
{
    ORION_NOT_IMPLEMENTED;
    return 0;
}
void *ShellExecuteA(
    void * /*unused*/,
    const char * /*unused*/,
    const char * /*unused*/,
    const char * /*unused*/,
    const char * /*unused*/,
    int /*unused*/)
{
    ORION_NOT_IMPLEMENTED;
    return nullptr;
}
int GetProfileStringA(
    const char * /*unused*/,
    const char * /*unused*/,
    const char * /*unused*/,
    char * /*unused*/,
    int /*unused*/)
{
    ORION_NOT_IMPLEMENTED;
    return 0;
}
// Thread
void KillTimer(void * /*unused*/, unsigned int /*unused*/)
{
    ORION_NOT_IMPLEMENTED;
}
void SetTimer(
    void * /*unused*/, unsigned int /*unused*/, unsigned int /*unused*/, void * /*unused*/)
{
    ORION_NOT_IMPLEMENTED;
}

#endif
