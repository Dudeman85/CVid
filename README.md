# CVid

This is the implementation of the paper *3D Graphics in CMD*.

It features a custom 3D render pipeline and rasterizer, as well as a unique output window in the form of the Windows console.
Basic model and texture loading are also implemented.

## Running The Demos
There are precompiled demos in the releases. To run them, simply unzip and run the executables.

You can also build them from souce. Just make sure the CVID_BUILD_DEMOS Cmake option is set.

### !!IMPORTANT!! 
Because Windows Terminal doesn't support resizing, these demos only work on the legacy Windows Console Host. 
If you're on **Windows 11** you need to change the default terminal application to Windows Console Host. This can be done by:
1. Open Windows Terminal
2. Click on the down arrow and select *Settings*
3. In the left pane of settings, select *Startup*
4. In the *Default terminal application* drop-down menu, select Windows Console Host

## Compiling
Build with Cmake and compile with Visual Studio. Requires C++ 23 or greater.

For any programs using the seperate console window, place app/ConsoleWindowApp.exe alongside the main executable.

## External Libraries used
- [tinyobjloader](https://github.com/tinyobjloader/tinyobjloader)
- [stb_image](https://github.com/nothings/stb)
- [Quaternions](https://github.com/ferd36/quaternions)