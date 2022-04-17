# geometry
Made with Raylib.

## Installation
### Windows

Download the geometry source code and extract the .zip file.
In the extracted folder, create a folder named `include` and a folder named `lib`.

Download and compile Raylib:

Download [w64devkit](https://github.com/skeeto/w64devkit/releases/download/v1.11.0/w64devkit-1.11.0.zip) and unzip the folder.
On your Windows system, type `env` in the search box and click Enter. Click on `Environment Variables`. Under `System Variables`, click the Path variable. In the new dialog window, copy the pathname of your extracted w64devkit folder and paste it into a new path.

You should now have a finished g++ compiler. To prove this, search for `cmd` in the Windows search box and press Enter. Run `g++ --version`.

Download and unzip [Raylib](https://github.com/raysan5/raylib).

From the command line, navigate to your raylib folder:
```
cd <path to your Raylib folder>/src
```
and run
```
mingw32-make PLATFORM=PLATFORM_DESKTOP
```

Copy the `libraylib.a` file from the `raylib/src` folder to your initially created `lib` folder and copy the `raylib.h` file to your `include` folder.

To create the final geometry.exe, navigate to your geometry folder on the command line and run:

```
g++ main.cpp -o geometry.exe -O1 -Wall -Wno-missing-braces -I include/ -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm
```

Or just follow this [tutorial](https://www.youtube.com/watch?v=HPDLTQ4J_zQ).
