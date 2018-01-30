
// Utf8FileSystem.cpp
// ==================
//
// Cross-platform library to work with files using utf8-encoded narrow strings as paths. See header file for full story.
//
// Written by Petr Ohlidal; See bottom of the file for license.


#include "Utf8FileSystem.h"

#ifdef _MSC_VER
    #include "windows.h"
#endif

#include <string>
#include <vector>

// ---------------------- Internal helpers -------------------------

namespace u8fs {

static ErrorInfo err;

void ClearError()
{
    err.type = Error::NONE;
    err.raw_result = 0;
}

#ifdef _MSC_VER

std::wstring MSW_Utf8ToWchar(const char* path) // Inspired by https://stackoverflow.com/a/3999597
{
    if( path == nullptr || path[0] == 0 )
    {
        err.type = Error::PATH_NULL_OR_EMPTY;
        return std::wstring();
    }

    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &path[0], -1, NULL, 0); // Doc: https://msdn.microsoft.com/en-us/library/windows/desktop/dd319072(v=vs.85).aspx
    std::wstring out_wstr( size_needed, 0 );
    err.raw_result = MultiByteToWideChar(CP_UTF8, 0, &path[0], -1, &out_wstr[0], size_needed);
    if (err.raw_result <= 0)
    {
        err.type = Error::CONV_TO_WCHAR_FAILED;
    }
    return std::move(out_wstr);
}

DWORD MSW_GetFileAttrs(const char* path)
{
    if (path == nullptr || path[0] == 0)
    {
        err.type = Error::PATH_NULL_OR_EMPTY;
        return INVALID_FILE_ATTRIBUTES;
    }

    std::wstring wpath = MSW_Utf8ToWchar(path);
    // Function reference: https://msdn.microsoft.com/en-us/library/windows/desktop/aa364944(v=vs.85).aspx
    // File attribute constants: https://msdn.microsoft.com/en-us/library/windows/desktop/gg258117(v=vs.85).aspx
    return GetFileAttributesW(wpath.c_str());
}

#endif // _MSC_VER

// ---------------------- Public interface -------------------------

#ifdef _MSC_VER
const char* PATH_SLASH = "\\";
#else
    #error "NOT IMPLEMENTED"
#endif


ErrorInfo GetLastError()
{
    return err;
}

bool HasError()
{
    return err.type != Error::NONE;
}

bool IsDirectory(const char* path) //!< Checks if the path is an existing directory.
{
    ClearError();
#ifdef _MSC_VER
    DWORD file_attrs = MSW_GetFileAttrs(path);
    return (file_attrs != INVALID_FILE_ATTRIBUTES && (file_attrs & FILE_ATTRIBUTE_DIRECTORY));
#else
    #error "Not implemented"
#endif
}

bool IsPlainFile(const char* path) //!< Checks if the file (not a directory) exists.
{
    ClearError();
#ifdef _MSC_VER
    DWORD file_attrs = MSW_GetFileAttrs(path);
    return (file_attrs != INVALID_FILE_ATTRIBUTES && ! (file_attrs & FILE_ATTRIBUTE_DIRECTORY));
#else
    #error "Not implemented"
#endif
}

bool PathExists(const char* path) //!< Checks if the path represents a file of any type (including a directory).
{
    ClearError();
#ifdef _MSC_VER
    return MSW_GetFileAttrs(path) != INVALID_FILE_ATTRIBUTES;
#else
    #error "Not implemented"
#endif
}

#if defined(_MSC_VER) && defined(CreateDirectory)
    #undef CreateDirectory
#endif

bool CreateDirectory(const char* path)
{
    ClearError();
#ifdef _MSC_VER
    std::wstring wpath = MSW_Utf8ToWchar(path);
    if (HasError())
    {
        return false;
    }
    err.raw_result = CreateDirectoryW(wpath.c_str(), nullptr);
    err.type = (err.raw_result == 0) ? Error::UNKNOWN : Error::NONE;
    return (err.raw_result != 0);
#else
    #error "Not implemented"
#endif
}

FILE* OpenFile(const char* path, const char* mode)
{
    ClearError();
#ifdef _MSC_VER
    std::wstring wpath = MSW_Utf8ToWchar(path);
    std::wstring wmode = MSW_Utf8ToWchar(mode);
    if (HasError())
    {
        return nullptr;
    }
    if (wmode.find(L"b") == std::wstring::npos) // When not binary...
    {
        wmode.pop_back(); // Remove NUL terminator - necessary for `append()` to take effect.
        wmode.append(L", ccs=UTF-8"); // Force UTF-8 encoding
    }
    FILE* f = nullptr;
    // fopen/_wfopen + mode strings: https://msdn.microsoft.com/en-us/library/yeby3zcb.aspx
    // fopen_s/_wfopen_s: https://msdn.microsoft.com/en-us/library/z5hh6ee9.aspx
    err.raw_result = _wfopen_s(&f, wpath.c_str(), wmode.c_str());
    if (err.raw_result != 0)
    {
        err.type = Error::UNKNOWN;
        return nullptr;
    }
    return f;
#else
    #error "Not implemented"
#endif
}

bool WriteTextToFile(FILE* f, const char* text)
{
    ClearError();
#ifdef _MSC_VER
    std::wstring wtext = MSW_Utf8ToWchar(text);
    if (HasError())
    {
        return false;
    }
    _set_errno(0); // Clear errno, see https://docs.microsoft.com/en-us/cpp/c-runtime-library/errno-doserrno-sys-errlist-and-sys-nerr
    err.raw_result = fwprintf(f, L"%s", wtext.c_str()); // https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/fprintf-fprintf-l-fwprintf-fwprintf-l
    if (errno != 0)
    {
        err.raw_result = errno;
        err.type = Error::UNKNOWN;
        return false;
    }
    return true;
#else
    #error "Not implemented"
#endif
}

} // namespace u8fs

/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2018 Petr Ohlidal
Permission is hereby granted, free of charge, to any person obtaining a copy of 
this software and associated documentation files (the "Software"), to deal in 
the Software without restriction, including without limitation the rights to 
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
of the Software, and to permit persons to whom the Software is furnished to do 
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this 
software, either in source code form or as a compiled binary, for any purpose, 
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this 
software dedicate any and all copyright interest in the software to the public 
domain. We make this dedication for the benefit of the public at large and to 
the detriment of our heirs and successors. We intend this dedication to be an 
overt act of relinquishment in perpetuity of all present and future rights to 
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN 
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION 
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/

