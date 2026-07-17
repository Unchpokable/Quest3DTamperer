# Quest3DTamperer
**PLEASE NOTE THAT THIS IS STILL IN VERY EARLY DEVELOPMENT!**

This is a tool made for messing with Quest3D games (mainly Audiosurf) to inspect and hopefully learn about their inner workings.

![grafik](https://user-images.githubusercontent.com/42943070/121410792-40245400-c963-11eb-8816-40b3f8dfd06c.png)

# DISCLAIMER
This tool is not supposed to:
- Enable piracy (There's way better ways for that than using this tool anyway)
- Enable cheating online (The tool can't even do this in its current state)
- Cause thermonuclear war

# Features
- Show information about channel groups (Name, protection and read-only status, file path)
- Remove protection and read-only status from channel groups and save them, allowing them to be opened by the Quest3D editor for inspection or editing.
- Load .cgr files into the game (**NOTE:** Not tested properly, don't count on it working)
- Read (and change) strings of some channels (Support added for the most common ones)
- Read and modify script source code of Lua script channels
- Extract textures
- Ability to write channel groups as [DOT files](https://en.wikipedia.org/wiki/DOT_(graph_description_language)) to visualize them as a digraph

More to come soon-ish.

# Dependencies
- The Quest3D 4.0 SDK (4.2 might work too) - proprietary, **not included in this repo**, see "Building" below
- [UGraphViz](https://github.com/Ubpa/UGraphviz) - vendored in `Quest3DTamperer/third_party/UGraphviz`
- [Dear ImGui](https://github.com/ocornut/imgui) - vendored in `Quest3DTamperer/third_party/imgui`
- [Detours](https://github.com/microsoft/Detours) - git submodule at `Detours/`, built from source (all D3D9/Quest3D
  hooking goes through it directly - see `Quest3DTamperer/src/hooks/`)
- The [DirectX SDK, June 2010](https://www.microsoft.com/en-us/download/details.aspx?id=6812) - discontinued by
  Microsoft, **not included in this repo**, see "Building" below. Headers only (needed for `d3dx9.h`/`d3dx9math.h`,
  which the Windows SDK no longer ships; nothing in this project calls an actual D3DX9 function, so none of its
  import libraries are linked)

Graphviz itself is **not** a dependency, despite UGraphViz's name - it only generates DOT source as plain strings.

# Building
Requires Visual Studio 2022+ (for the MSVC toolset), CMake 3.21+, and Ninja (ships with Visual Studio). This project
targets x86/Win32 only - Audiosurf is a 32-bit process, so a 64-bit build of this DLL could never be injected into it.

1. Fetch the Detours submodule:
   ```
   git submodule update --init --recursive
   ```
2. Get a copy of the DirectX SDK, June 2010. Microsoft no longer distributes it; an archival mirror is available at
   [testing-laboratory/DirectX-SDK-June2010](https://github.com/testing-laboratory/DirectX-SDK-June2010). Drop it at
   `DirectX-SDK-June2010/` or point `DXSDK_DIR` at wherever you put it.
3. Get a copy of the Quest3D SDK yourself (see the disclaimer above) and either drop it at
   `Quest3D-SDK/` or point `Q3D_SDK_DIR` at wherever you put it.
4. From an **x86 Native Tools Command Prompt for VS**, in the repo root:
   ```
   cmake --preset x86-release -DQ3D_SDK_DIR="C:/path/to/Quest3D SDK" -DDXSDK_DIR="C:/path/to/DirectX SDK"
   cmake --build --preset x86-release
   ```
   (Omit `-DQ3D_SDK_DIR`/`-DDXSDK_DIR` if you used the default `Quest3D-SDK/`/`DirectX-SDK-June2010/` locations.) The
   resulting `Quest3DTamperer.dll` ends up in `build/x86-release/bin/`. An `x86-debug` preset is also available.
