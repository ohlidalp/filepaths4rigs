
#include "../Utf8FileSystem.h"

#include <stdio.h>

// ~ Příšerně žluťoučký kůň úpěl ďábelské ódy ~
// "Terribly yellow-ish horse sung devillish odes" = A pangram for testing Czech special characters
// Written using escapes for safety.

//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ |Příšerně                    |žluťoučký                    |kůň           |úpěl
static const wchar_t* TEST_PANGRAM_WCHAR = L"P\u0159\u00ED\u0161ern\u011B \u017Elu\u0165ou\u010Dk\u00FD k\u016F\u0148 \u00FAp\u011Bl";
static const char*    TEST_PANGRAM_UTF8 = u8"P\u0159\u00ED\u0161ern\u011B \u017Elu\u0165ou\u010Dk\u00FD k\u016F\u0148 \u00FAp\u011Bl";

// USED UNICODE CHARACTERS
// 'ř' = 'LATIN SMALL LETTER R WITH CARON' (U+0159)       http://www.fileformat.info/info/unicode/char/159/index.htm
// 'í' = 'LATIN SMALL LETTER I WITH ACUTE' (U+00ED)       http://www.fileformat.info/info/unicode/char/00ed/index.htm
// 'š' = 'LATIN SMALL LETTER S WITH CARON' (U+0161)
// 'ň' = 'LATIN SMALL LETTER N WITH CARON' (U+0148)
// 'ě' = 'LATIN SMALL LETTER E WITH CARON' (U+011B)
// 'ž' = 'LATIN SMALL LETTER Z WITH CARON' (U+017E)       http://www.fileformat.info/info/unicode/char/017e/index.htm
// 'ť'´= 'LATIN SMALL LETTER T WITH CARON' (U+0165)       https://www.fileformat.info/info/unicode/char/0165/index.htm
// 'č' = 'LATIN SMALL LETTER C WITH CARON' (U+010D)       http://www.fileformat.info/info/unicode/char/10d/index.htm
// 'ý' = 'LATIN SMALL LETTER Y WITH ACUTE' (U+00FD)       http://www.fileformat.info/info/unicode/char/00fd/index.htm
// 'ů' = 'LATIN SMALL LETTER U WITH RING ABOVE' (U+016F)  http://www.fileformat.info/info/unicode/char/016f/index.htm
// 'ň' = 'LATIN SMALL LETTER N WITH CARON' (U+0148)       http://www.fileformat.info/info/unicode/char/0148/index.htm
// 'ú' = 'LATIN SMALL LETTER U WITH ACUTE' (U+00FA)       http://www.fileformat.info/info/unicode/char/00FA/index.htm

int main()
{
    // Create directory
    char dirname[500];
    sprintf(dirname, "Directory_[%s]", TEST_PANGRAM_UTF8);
    if (!u8fs::CreateDirectory(dirname))
    {
        printf("Could not create directory\n");
        return -1;
    }
    if (u8fs::HasError())
    {
        printf("!! Silent failure when creating directory\n");
        return -1;
    }

    // Verify directory
    if (!u8fs::IsDirectory(dirname))
    {
        printf("Could not find created directory\n");
        return -1;
    }
    if (u8fs::HasError())
    {
        printf("Failure when checking directory\n");
        return -1;
    }

    // Try checking non-existent directory
    char baddir[500];
    sprintf(baddir, "%s_nope", dirname);
    if (u8fs::IsDirectory(baddir))
    {
        printf("!! Error: IsDirectory() failed and confirmed non-existent directory!!\n");
        return -1;
    }
    if (u8fs::HasError())
    {
        printf("Failure when checking non-existent directory\n");
        return -1;
    }

    // Create file in the directory
    char filepath[500];
    sprintf(filepath, "%s%sFile_[%s]", dirname, u8fs::PATH_SLASH, TEST_PANGRAM_UTF8);
    FILE* f = u8fs::OpenFile(filepath, "w");
    if (f == nullptr)
    {
        printf("Could not open file (got empty handle)\n");
        return -1;
    }
    if (u8fs::HasError())
    {
        printf("!! Silent failure when opening file\n");
        return -1;
    }

    // Write text to the file
    fwprintf(f, L"manual fwprintf(): [%s]\n", TEST_PANGRAM_WCHAR);
    char buf[500];
    sprintf(buf, "WriteTextToFile(): [%s]\n", TEST_PANGRAM_UTF8); 
    u8fs::WriteTextToFile(f, buf);

    // Close the file
    if (fclose(f) != 0)
    {
        printf("Warning: fclose() failed\n");
    }

    // Check we can find the file
    if (!u8fs::IsPlainFile(filepath))
    {
        printf("Could not verify the created file\n");
        return -1;
    }
    if (u8fs::HasError())
    {
        printf("Failure when checking the created file\n");
        return -1;
    }

    // Check we cannot find non-existent file
    char badpath[500];
    sprintf(badpath, "%s_nope", filepath);
    if (u8fs::IsPlainFile(badpath))
    {
        printf("!! Error: IsPlainFile() failed and confirmed non-existent file!!\n");
        return -1;
    }
    if (u8fs::HasError())
    {
        printf("Failure when checking non-existent file\n");
        return -1;
    }

    // Try listing the current dir
    std::vector<std::string> files;
    if (!u8fs::ListDirectory(files, "."))
    {
        printf("ListDirectory() failed\n");
        return -1;
    }
    else
    {
        printf("Files in current directory:\n");
        for (std::string& fname: files)
        {
            printf("\t%s\n", fname.c_str());
        }
    }

    printf("Hit ENTER to continue...");
    getchar();
}

