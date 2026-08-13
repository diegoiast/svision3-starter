# SVision3 default example

This is a startup project for SVision3. You should fork it under another name and
continue the development of your application.

What is SVision3? Its a cross platform GUI toolkit written in C++. It contains its
own widgets theme, sqlite3 support and networking. For full features list visit
[the github repo for SVision3 - https://github.com/diegoiast/svision3](https://github.com/diegoiast/svision3).

The `main` branch of this repo will contain a single commit. Ideally you should
`git commit --amend --reset-author` to remove this commit, and the `dev` branch.

If you are interested in developing the default example (newer features, better docs
other CI/CD - the development is done on `dev` branch.

## Features

1. C++/CMake/Conan2 structure.
2. Basic hello world app in C++ 17/20.
3. Unit tests using catch2.
4. Clang format.
5. CI/CD that configures/builds + creates installer
    1. Github
    2. Gitlab
    3. Azure piplines

This demo is licensed under the MIT license, so you can
later on choose a more suitable license (closed source of GPL).

## Prerequisites

- CMake 3.27+
- [Conan 2.x](https://docs.conan.io/2/installation.html)
- A C++20 compiler (GCC 12+, Clang 15+, MSVC 2022+)

After installing Conan, you may need to generate a default profile so it can
detect your compiler and platform settings:

```bash
conan profile detect
```

This creates a default profile (`~/.conan2/profiles/default` on Linux/macOS,
`%USERPROFILE%\.conan2\profiles\default` on Windows) with auto-detected values.
You only need to do this once per machine.

On Linux also install the system packages for display and graphics:

```text
# Debian/Ubuntu
sudo apt install libgl-dev libx11-dev libxext-dev \
                 libwayland-dev libxkbcommon-dev libdbus-1-dev \
                 libcairo2-dev libfontconfig1-dev pkg-config

# Fedora
sudo dnf install mesa-libGL-devel libX11-devel libXext-devel \
                 wayland-devel libxkbcommon-devel dbus-devel \
                 cairo-devel fontconfig-devel pkgconfig

# Arch Linux
sudo pacman -S mesa libx11 libxext wayland libxkbcommon \
               dbus cairo fontconfig pkgconf
```

## Install dependencies and build the example

```bash
cd myapplication

# Debug
conan install . -s build_type=Debug
cmake --preset conan-debug
cmake --build build/Debug

# Release
conan install . -s build_type=Release
cmake --preset conan-release
cmake --build build/Release
```

On Windows with MSVC the commands are the same — Conan detects the compiler
and generates the appropriate toolchain automatically.

> *NOTE*
>
> You **must** modify the GUIDs in CMakeLists.txt. Otherwise your product
> **will break in production**.
>
> You should also change the license probably.
>
> You should also disable CICD for platforms you do not need to support.

## Running tests

After building, run the test suite with CTest:

```bash
ctest --test-dir build/Debug -C Debug --output-on-failure
```

Tests live in `tests/`. The example in `tests/test_example.cpp` uses
[Catch2](https://github.com/catchorg/Catch2) — replace it with tests for
your own application logic.

## Subsequent builds

After the first setup, only the cmake build step is needed:

```bash
cmake --build build/Debug
```

Re-run `conan install` only when `conanfile.py` changes (e.g. after a
svision3 version bump).

## Helping with developing this template

`main` of the template project always contains a single commit - always. It gets
squashed "once in a while" from `develop` branch. All code is beeing develped
there.

All PRs should be done against the `develop` branch.
