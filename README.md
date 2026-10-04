# AntiFallah

> One purpose: prevent unauthorized Windows installations on school PCs.

AntiFallah is a small Windows utility written in C++ that detects Windows installation media connected to the computer and intentionally corrupts its installation image.

It searches available Windows drive letters for:

* `\sources\install.wim`
* `\sources\install.esd`

When one of these files is found, AntiFallah removes exactly 100 MB from it. This makes the affected Windows installation image unusable.

The project was created as a humorous experiment for people who want to prevent unauthorized Windows installations on computers they manage.

## Warning

AntiFallah modifies files destructively.

Do not run it on a computer, drive, or installation medium unless you have explicit permission to modify it. The program does not create a backup, and the damaged installation image may not be recoverable.

It may also affect non-removable drives if they contain a matching Windows installation path.

## How It Works

1. AntiFallah checks the available Windows drive letters.
2. It searches for `install.esd` and `install.wim` inside each drive's `sources` directory.
3. When a matching file is found, it opens the file with read/write access.
4. It removes 100,000,000 bytes from a selected section of the file.
5. The program exits after processing the first matching installation image.

If the requested byte range is outside the file, AntiFallah removes the final 100 MB instead.

## Requirements

* Windows
* MinGW-w64 GCC
* GNU C++ compiler
* MinGW-w64 Winpthreads
* Standard C++ library
* `windows.h`

The project can be built using MSYS2's UCRT64 environment.

## Installing the Build Environment

The recommended build environment is MSYS2 with the UCRT64 toolchain.

Install MSYS2 and open the `MSYS2 UCRT64` terminal.

Install the MinGW-w64 GCC toolchain with:

```bash
pacman -S mingw-w64-ucrt-x86_64-gcc
```

The GCC toolchain includes the required MinGW-w64 runtime components, including Winpthreads dependencies.

If Winpthreads needs to be installed explicitly, use:

```bash
pacman -S mingw-w64-ucrt-x86_64-winpthreads
```

The package provides the MinGW-w64 Winpthreads library and its pthread headers and libraries.

Verify the compiler installation with:

```bash
g++ --version
```

## Building

Compile with:

```bash
g++ main.cpp -o AntiFallah.exe -std=c++17 -mwindows -static-libgcc -static-libstdc++
```

If your MinGW-w64 distribution requires explicit Winpthreads linking, use:

```bash
g++ main.cpp -o AntiFallah.exe -std=c++17 -mwindows -static-libgcc -static-libstdc++ -static -lwinpthread
```

The `-mwindows` option prevents a console window from appearing.

Build options can vary between MinGW-w64 distributions. Test the resulting executable in an isolated environment before using it.

## Installation

The repository may include a batch file intended to copy the executable into the current user's Windows Startup folder.

Before using any startup mechanism, make sure that:

* You own or administer the computer.
* All users have been informed.
* The program's destructive behavior is clearly documented.
* You have tested it with disposable media.
* You have a removal and recovery procedure.

The startup script should refer to `AntiFallah.exe`, not an unrelated or deceptive executable name.

For managed school computers, a transparent administrator-controlled deployment method is recommended instead of disguising the program as another application.

## Limitations

* AntiFallah only searches for `install.wim` and `install.esd`.
* It does not verify that the files belong to Windows 11.
* It does not distinguish between official and modified installation media.
* It only processes the first matching file it finds.
* It does not make backups.
* It may damage installation media that was intended for legitimate recovery or repair.
* It does not provide a user interface or confirmation prompt.
* It is not a complete school-computer management or security solution.

## Recommended Improvements

Possible future improvements include:

* Add an explicit confirmation or administrator-controlled configuration.
* Restrict scanning to removable drives.
* Detect USB devices instead of repeatedly scanning every drive letter.
* Create a log file.
* Provide a safe dry-run mode.
* Add a recovery or backup mechanism.
* Allow administrators to configure the target file size and scan interval.
* Use a transparent Windows policy or device-management solution instead of modifying installation media.

## License

AntiFallah is free software distributed under the GNU General Public License, version 3 or later.

See the `LICENSE` file for the complete license text.

## Author

Copyright © 2026 Muhammad Hossein.

## Disclaimer

This software is provided for educational, experimental, and authorized administrative use only.

The author is not responsible for:

* Lost or damaged data
* Broken installation media
* Unbootable recovery drives
* System interruptions
* Unauthorized use of the software
* Any consequences caused by deploying it on computers or storage devices

Use it only on systems and media that you are authorized to modify.
