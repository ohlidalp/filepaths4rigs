
// Utf8FileSystem.h
// ================
//
// Simplistic and minimal library for working with files using utf8-encoded narrow strings as paths.
// Designed primarily as educational example - performance is ignored, minimum input/error checking is done.
// Intended use: include the files in your project and modify/extend as necessary.
// Datatype `char` and subsequently `std::string` are considered to always be UTF-8 - inspired by http://utf8everywhere.org/
//
// Written by Petr Ohlidal; See bottom of the file for license.

#pragma once

#ifdef _MSC_VER // MS Visual Studio, see https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros
    #include <wchar.h> // FILE*
#endif

namespace u8fs
{
    extern const char* PATH_SLASH;

    enum class Error
    {
        NONE = 0,
        UNKNOWN,
        CONV_TO_WCHAR_FAILED,
        PATH_NULL_OR_EMPTY
    };

    struct ErrorInfo
    {
        Error type;
        int raw_result;
    };

    ErrorInfo GetLastError();                   //!< Returns info about previous call
    bool HasError();                            //!< Informs whether GetLastError() contains error info

    bool IsDirectory(const char* path);         //!< Checks if the path is an existing directory.
    bool IsPlainFile(const char* path);         //!< Checks if the file (not a directory) exists.
    bool PathExists(const char* path);          //!< Checks if the path represents a file of any type (including a directory).

    bool CreateDirectory(const char* path);

    /// Supported modes: "[b]r/w/a[+]"
    /// To close the handle, use ordinary `fclose()` from "stdio.h" (works OK on MSWindows, too)
    /// MSWindows:
    ///  * The returned handle must be used only with wchar functions, like `fwprintf()`
    ///  * If file is not binary, encoding will be forced to UTF-8
    FILE* OpenFile(const char* path, const char* mode);

    /// Convenience for MSWindows, allows writing UTF-8 narrow text to file
    bool WriteTextToFile(FILE* f, const char* text);


} // namespace

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

