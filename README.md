# filepaths4rigs

Character sets/encodings of filesystem paths are a shady area. Even C++17 filesystem library will just tell you "[it's OS-dependent/unspecified/`${OTHER_EXCUSE}`](http://en.cppreference.com/w/cpp/filesystem/path/string)".

This project is my research testbed. It's named "\*4rigs" after my pet-project [Rigs of Rods](https://github.com/only-a-ptr/rigs-of-rods).

# Goal

To develop a tiny file-manipulation library using only UTF-8 encoded narrow strings as paths.

# Status

Demo code for windows, reading for \*nix. See [Windows notes](/doc/MS_WINDOWS.md) and [Linux/Unix notes](/doc/UNIX_LINUX.md).

# License

Dual-licensed as either public domain or MIT, see LICENSE.txt.

This approach is inspired by STB, see https://github.com/nothings/stb#why-public-domain.
