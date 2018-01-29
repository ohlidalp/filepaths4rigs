
#include "stdafx.h"

// === Microsoft Windows OS ====
// What happens if you have files with Unicode names and you list directory using ANSI functions?
// How is the conversion determined? I was unable to find a clear answer in MSDN.

// NOTE: This file MUST BE Unicode (UTF8 or other).
//       Saved using VisualStudio2015 as 'UTF8 without signature, codepage 65001'
// Note that UTF8 signature is generally frowned upon: https://stackoverflow.com/a/2223926

// UNICODE CHARACTERS
// 'ř' = 'LATIN SMALL LETTER R WITH CARON' (U+0159) http://www.fileformat.info/info/unicode/char/159/index.htm
// 'í' = 'LATIN SMALL LETTER I WITH ACUTE' (U+00ED) http://www.fileformat.info/info/unicode/char/00ed/index.htm
// 'š' = 'LATIN SMALL LETTER S WITH CARON' (U+0161)
// 'ň' = 'LATIN SMALL LETTER N WITH CARON' (U+0148)
// 'ě' = 'LATIN SMALL LETTER E WITH CARON' (U+011B)

// ~ Příšerně žluťoučký kůň úpěl ďábelské ódy ~
// "Terribly yellow-ish horse sung devillish odes" = A pangram for testing Czech special characters
// Written using escapes, just to be double sure.
static const wchar_t* TEST_PANGRAM = L"P\u0159\u00ED\u0161ern\u011B";

void MyCloseHandle(HANDLE& h)
{
    if (h == INVALID_HANDLE_VALUE)
        return;

    if (CloseHandle(h) == FALSE)
    {
        std::cout << "CloseHandle() failed, GetLastError():" << GetLastError() << std::endl;
    }
    h = INVALID_HANDLE_VALUE;
}

void MyFindClose(HANDLE& h)
{
    if (h == INVALID_HANDLE_VALUE)
        return;

    if (FindClose(h) == FALSE)
    {
        std::cout << "FindClose() failed, GetLastError():" << GetLastError() << std::endl;
    }
    h = INVALID_HANDLE_VALUE;
}

void MyWriteHexA(const char* str)
{
    std::cout << " * | hex| dec" << std::endl;
    std::cout << "---+----+----" << std::endl;
    size_t len = strlen(str);
    for (size_t i = 0; i < len; ++i)
    {
        uint32_t c32 = str[i];
        uint32_t c_masked = c32 & 0x000000ff; // Get only last 1 byte (= least signifficant byte)
        wprintf(L" %c | %2x | %3d\n", c_masked, c_masked, c_masked);
    }
}

void MyWriteHexW(const wchar_t* str)
{
    std::cout << " * |  hex | dec" << std::endl;
    std::cout << "---+------+----" << std::endl;
    size_t len = wcslen(str);
    for (size_t i = 0; i < len; ++i)
    {
        wprintf(L" %c | %4x | %6d \n", str[i], str[i], str[i]);
    }
}

void MyCheckCodepages()
{
    UINT cur_ansi_cp = GetACP();
    CPINFOEXA cp_info;
    GetCPInfoExA(cur_ansi_cp, 0, &cp_info);
    std::cout << "Current ANSI codepage: name=["<<cp_info.CodePageName<<"], id="<<cur_ansi_cp<<std::endl;
    printf("\t printf(): %s\n", cp_info.CodePageName); // Aren't there any conversions?

    UINT cur_oem_cp = GetOEMCP();
    GetCPInfoExA(cur_oem_cp, 0, &cp_info);
    std::cout << "Current OEM codepage: name=["<<cp_info.CodePageName<<"], id="<<cur_oem_cp<<std::endl;
    printf("\t printf(): %s\n", cp_info.CodePageName); // Aren't there any conversions?
}

/// @param wstr _must_ be NUL-terminated!
/// @author Taken from https://stackoverflow.com/a/3999597
int CalcUtf8Size(const wchar_t* wstr)
{
    if (wstr == nullptr || wstr[0] == 0) // NULL or empty string
    {
        return 0;
    }
    else
    {
        // Docs: https://msdn.microsoft.com/en-us/library/windows/desktop/dd374130(v=vs.85).aspx
        return WideCharToMultiByte(CP_UTF8, 0, &wstr[0], -1, NULL, 0, NULL, NULL);
    }
}

/// @param wstr _must_ be NUL-terminated!
/// @author Taken from https://stackoverflow.com/a/3999597
bool WcharToUtf8(char* dst, int dst_size, const wchar_t* wstr)
{
    if (dst_size == -1)
    {
        dst_size = CalcUtf8Size(wstr);
    }
    // Docs: https://msdn.microsoft.com/en-us/library/windows/desktop/dd374130(v=vs.85).aspx
    return (0 < WideCharToMultiByte(CP_UTF8, 0, &wstr[0], -1, &dst[0], dst_size, NULL, NULL));
}

/// @param wstr _must_ be NUL-terminated!
/// @author Taken from https://stackoverflow.com/a/3999597
std::string WcharToUtf8(const wchar_t* wstr)
{
    const int dst_size = CalcUtf8Size(wstr);
    std::string dst(dst_size, 0); // Construct by length and initial value
    WcharToUtf8(&dst[0], dst_size, wstr);
    return std::move(dst);
}

// Convert an UTF8 string to a wide Unicode String
// Taken from https://stackoverflow.com/a/3999597
std::wstring MyUtf8ToWchar(const std::string &str)
{
    if( str.empty() )
        return std::wstring();

    // MultiByteToWideChar(): https://msdn.microsoft.com/en-us/library/windows/desktop/dd319072(v=vs.85).aspx
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0); // The initial "Pangram_Příšerně" => 17 bytes
    std::wstring out_wstr( size_needed, 0 );
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &out_wstr[0], size_needed);
    return out_wstr;
}

int main()
{

    wchar_t horsefile[1000];
    wsprintf(horsefile, L"Pangram_%s", TEST_PANGRAM);

    HANDLE h = CreateFileW(horsefile, (GENERIC_READ | GENERIC_WRITE), 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE)
    {
        std::cout << "Failed to create file 'Pangram_', GetLastError():" << GetLastError() << std::endl;
    }
    MyCloseHandle(h);

    // "Listing the Files in a Directory": https://msdn.microsoft.com/en-us/library/windows/desktop/aa365200(v=vs.85).aspx

    MyCheckCodepages();

    WIN32_FIND_DATAA found; // ANSI version, https://msdn.microsoft.com/en-us/library/windows/desktop/aa365740(v=vs.85).aspx
    h = FindFirstFileA("Pangram*", &found);
    if (h == INVALID_HANDLE_VALUE)
    {
        std::cout << "Failed to FindFirstFileA(), GetLastError():" << GetLastError() << std::endl;
    }
    else
    {
        std::cout <<std::endl << "FindFirstFileA() -> filename: [" << found.cFileName << "]" << std::endl;
        MyWriteHexA(found.cFileName);
    }

    WIN32_FIND_DATAW found_w; // Wide version, https://msdn.microsoft.com/en-us/library/windows/desktop/aa365740(v=vs.85).aspx
    h = FindFirstFileW(L"Pangram*", &found_w);
    if (h == INVALID_HANDLE_VALUE)
    {
        std::cout << "Failed to FindFirstFileW(), GetLastError():" << GetLastError() << std::endl;
    }
    else
    {
        wprintf_s(L"FindFirstFileW() -> filename [%s]\n", found_w.cFileName);
        MyWriteHexW(found_w.cFileName);

        // UTF-8 conversion test
        std::string u8str = WcharToUtf8(found_w.cFileName);
        std::cout << "The above, UTF-8 converted: [" << u8str << "]" <<std::endl;
        printf("\tprintf(): [%s]\n", u8str.c_str());
        MyWriteHexA(u8str.c_str());

        // Reverse conversion to widechar
        std::wstring u16str = MyUtf8ToWchar(u8str);
        // Compare with original
        if (wcscmp(found_w.cFileName, u16str.c_str()) == 0)
        {
            // They're equal allright :)
            std::cout << "UTF8<-->UTF16 conversion works OK" << std::endl;
        }
        else
        {
            // Dang! They're not equal for some reason :(
            std::cout << "  !!  Converted string is not equal to source string  !!  " << std::endl;
        }
    }


    system("pause");
}

