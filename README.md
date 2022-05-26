# geometry
Made with Raylib.

![screenshot](https://raw.githubusercontent.com/NO411/geometry/master/screenshot.png)

## Usage

| Keys                             | Action                                                                       |
|----------------------------------|------------------------------------------------------------------------------|
| `CTRL` and press `C`             | circle drawing mode                                                          |
| `CTRL` and press `S`             | straight line drawing mode                                                   |
| `CTRL` and press `D`             | distance drawing mode                                                        |
| `CTRL` and press `R`             | ray drawing mode                                                             |
| `CTRL` and press `P`             | point drawing mode                                                           |
| `CTRL` and press `E`             | enable eraser                                                                |
| `left mouse button`              | set first / second point of a line or the middle point or radius of a circle |
| `ESC`                            | Place/use                                                                    |
| `←`                              | move everything to the left                                                  |
| `→`                              | move everything to the right                                                 |
| `↑`                              | move everything up                                                           |
| `↓`                              | move everything down                                                         |
| `CTRL` + `D` and press `M`       | distance measurement mode                                                    |
| `CTRL` + `D` + `M` and press `E` | distance measurement eraser mode                                             |

The cursor automatically connects to intersections, other points, lines, and circles.

## Installation

### Linux

#### Ubuntu

Navigate to the folder where you want to install geometry.

```
cd <path to your installation folder>
```

Then run:

```
sudo apt install g++ git make libasound2-dev mesa-common-dev libx11-dev libxrandr-dev libxi-dev xorg-dev libgl1-mesa-dev libglu1-mesa-dev && git clone https://github.com/NO411/geometry && git clone https://github.com/raysan5/raylib && cd raylib/src && make PLATFORM=PLATFORM_DESKTOP RAYLIB_LIBTYPE=SHARED && sudo make install RAYLIB_LIBTYPE=SHARED && cd ../../geometry/src && g++ main.cpp math/math_misc.cpp -o ../geometry -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 && ./../geometry
```

To run the program after installing it in your installation folder, type:
```
cd geometry && ./geometry.exe
```

### Windows

Download the geometry source code and extract the .zip file.
In the extracted folder, create a folder named `include` and a folder named `lib`.

Download and compile Raylib:

Download [w64devkit](https://github.com/skeeto/w64devkit/releases/download/v1.11.0/w64devkit-1.11.0.zip) and unzip the folder.
On your Windows system, type `env` in the search box and click Enter. Click on `Environment Variables`. Under `System Variables`, click the Path variable. In the new dialog window, copy the pathname of your extracted `w64devkit/bin` folder and paste it into a new path.

You should now have a finished g++ compiler. To prove this, run the `w64devkit.exe` and search for `cmd` in the Windows search box and press Enter. Run `g++ --version`.

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
g++ main.cpp -o geometry.exe -O1 -Wall -Wno-missing-braces -I include/ -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm -mwindows
```

Or just follow this [tutorial](https://www.youtube.com/watch?v=HPDLTQ4J_zQ).
