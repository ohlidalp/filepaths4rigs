
# Notes on MS Windows

NTFS filesystem (default in all recent Windows) uses Unicode, see "[MSDN: Character Sets Used in File Names](https://msdn.microsoft.com/en-us/library/windows/desktop/dd317748(v=vs.85).aspx)"

Apparently `FindFirstFileA()` and probably all other `*A()` file path functions return garbage for Unicode paths. 
Specifically, they seem to convert the Unicode path using the currrent CP_ACP. The conversion is lossy and the result path
is useless for file access. This is what I got from my home PC using Czech localization:

TODO ADD IMAGE
