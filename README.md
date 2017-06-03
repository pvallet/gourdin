# gourdin

This project is a 3D/2D engine developped under SFML and OpenGL for educational
purposes.

For more details about its development, please refer to report.pdf (coming soon).

Contact: <luap.vallet@gmail.com>

---

Two modes are featured in gourdin, a Sandbox mode and a Game mode.

The **Sandbox** mode is made to be able to see the whole structure of the island and navigate through the different biomes easily. This is still a work in progress so there is still obvious work to be done on texture transitions and forests structure.

There are some antilope and deer herds scattered over the island, and appear only on certain biomes. The antilopes appear only on the biome with small plants with big leaves (making the environment more consistent is on the TO-DO list). The deers are easier to find.

As a remnant from the earlier development stages, you can hunt them by spawning predators with the right click in the adequate biomes, and control them as in a classic RTS. Up to you to find where you can spawn wolves, lions and leopards ;)

Any feedback on the structure of the island and the environment is much appreciated (the controls of the predators is not the main purpose of this mode).

The **Game** mode is made to show how would be the controls and the camera for a game using this engine. In this mode you control a small tribe that you can move around, switching focus among characters. There are two camera types, a 'God' camera and a point-of-view one.

You can move around using the mouse or the keyboard. The main purpose is to see if this type of controls would be suitable for portable devices.

Any feedback on the global ergonomy of the controls and camera would be much appreciated. If that also gives you gamedesign ideas, tell me about it!

## Usage

The controls are displayed in the main screen in gourdin under Linux and Windows. However, Mac OS requires a greater OpenGL version that is not compatible with SFML, hence the interface is absent.

This is why we also provide the controls here.

#### 1. Sandbox mode
The controls of the sandbox mode are pretty much the same as a RTS game (see Populous: The Beginning).

```
Esc: Quit game
M: Switch to Sandbox mode
Left-Right:  Rotate camera
Up-Down:     Go forwards/backwards
B:  Launch benchmark
L:  Hide/Display log
W:  Switch to wireframe display

Click on the minimap to jump there
Right-click to make a lion appear
Select it with the left mouse button
Move it around with the right button
Lshift:  Make it run
```

#### 2. Game mode

###### God camera
```
Esc: Quit game
M: Switch to Sandbox mode
1: God camera
2: POV camera
A-E:  Rotate camera
ZQSD: Move focused character
LShift+ZQSD: Change focused character to closest in given direction
(The game is optimised for AZERTY keyboards)

Click to move the character in the center
Click on another character to change the focus
Click and drag to rotate the camera
```

###### POV camera
The controls are the same but you move the camera around with the arrows (you can still use the mouse).

---

## Installation

On **Windows** you have two options:
* Download the [installer](https://github.com/pvallet/gourdin/releases/download/v0.1.0-rc/gourdin-windows-setup.exe) to install gourdin on your computer. You will be able to run it through the windows menu and create a desktop icon if you want. If you have an error telling you that you don't have the permissions to run it, right ctrl-click on it and choose "Run as administrator".
* Download [gourdin-windows-folder.zip](https://github.com/pvallet/gourdin/releases/download/v0.1.0-rc/gourdin-windows-folder.zip) that contains the directory with all the game info. You can unzip it and run it directly by double-clicking on `gourdin.exe`

On **Mac OS** you should download [gourdin-mac.app.zip](https://github.com/pvallet/gourdin/releases/download/v0.1.0-rc/gourdin-mac.app.zip). Then you only have to `ctrl-click` on it and choose open to allow executing from an unidentifed developer (me).

On **Linux** there is no pre-built package, you have to build it from source.

All the files are available here: https://github.com/pvallet/gourdin/releases/

---

## Building from source

The building is done with CMake to be cross-platform.

## Linux

#### 1. Dependencies

Firstly, install the dependencies for the building environment.
```
sudo apt-get install build-essential libgl1-mesa-dev libgomp1 cmake
```
Then you will need the dependencies of the project. Prefer using the ones from the repositories as building them from the source takes a while.
```
sudo apt-get install libsfml-dev libglew-dev libglm-dev
```

#### 2. Building the project

To build the project, simply run
```
mkdir build && cd build
cmake ..
make -j4
```
If cmake does not work because a dependency is not satisfied (e.g. SFML < 2.4.1), please refer to the section

3.(Optional) Build dependencies locally.

To run gourdin, go the root of the project (with `cd ..`) and then run the executable:
```
./build/bin/Release/out
```

Be sure to pick the right path, for example if you chose a debug build you might want to run `./build/bin/Debug/out`

If you want to configure Code::Blocks in order to use its debugger, please refer to the section

Windows -> 3.Configure your Code::Blocks project

#### 3. (Optional) Build dependencies locally

You might need to build these dependencies locally, for example if your repository does not have the right version (e.g. SFML < 2.4.1). In order to do that, run the script with no argument to get all of them.
```
./get_dependencies
```
You can also specify which libraries you want to build.
```
./get_dependencies SFML
```

The entries are `glm`, `SFML` and `glew`.

---

## Mac OS

**This section is to build the project from source, if you simply want to run it go to the _*Installation*_ section.**


#### 1. Homebrew

In order to easily install the dependencies, we recommend to use Homebrew, a package manager similar to apt-get in linux.

```
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```

#### 2. Dependencies

You then need CMake to generate your project and Clang (C++ compiler) to build it:
```
brew install --with-clang llvm
brew install cmake
```
Finally install the dependencies of the project:
```
brew install glew glm sfml
```

#### 3. Building the project

In order to build the project, please refer to the section Linux -> 2. Building the project.

Please note that cmake might throw some errors about testing OpenMP. You can ignore them, the CMake config file is not up to date with the compiler Clang, that features OpenMP natively.

---

## Windows

**This section is to build the project from source, if you simply want to run it go to the _*Installation*_ section.**


All the steps will be explained as if the user was using Code::Blocks. It is easy to adapt the tutorial to other IDEs.

#### 1. OpenMP

The first step is to use a compiler that supports OpenMP.

TDM-GCC does it pretty well on windows.

https://sourceforge.net/projects/tdm-gcc/files/TDM-GCC%20Installer/tdm-gcc-5.1.0-3.exe/download

Here is a tutorial on how to install it on windows. During the installation of TDM-GCC, **don't forget to check the OpenMP box in the gcc menu when choosing your components**.

http://wiki.codeblocks.org/index.php/MinGW_installation

#### 2. Dependencies

You have then to retrieve all the dependencies on github, and compile them with your compiler, so as to make them compatible with gourdin.

Coming soon - retrieve them easily with git submodules.

##### Get the dependencies

You will need SFML 2.4.1 or higher, GLEW and GLM

https://github.com/SFML/SFML/archive/2.4.1.zip

https://github.com/nigels-com/glew/releases/download/glew-2.0.0/glew-2.0.0.zip

https://github.com/g-truc/glm/releases/download/0.9.8.4/glm-0.9.8.4.zip

##### Unpack them

You will have to create a folder `external` in which you will unzip your dependencies.
Afterwards, rename the extracted folders so that your working directory (`gourdin`) looks like this:
```
gourdin
+-- external
|   +-- glew
|   |   +-- bin
|   |   +-- build
|   +-- glm
|   |   +-- doc
|   |   +-- glm
|   +-- SFML
|   |   +-- cmake
|   |   +-- include
+-- external_manual
...
```

##### Build them

This tutorial uses CMake to create the Code::Blocks projects used to build the libraries.
https://cmake.org/download/

Each time, you will need to run CMake-gui, specify where is the source code and where to build the libraries. Then, you have to click on `Configure` and select `CodeBlocks - MinGW Makefiles`, and finally click on `Generate`.

Then, you have to open the generated Code::Blocks project generated in the build directory, and click on `build`.

###### glew

```
Source code:  C:/path/to/gourdin/external/glew/build/cmake
Build folder: C:/path/to/gourdin/external/glew/build/
```

Then copy the folders `glew/build/bin` and `glew/build/lib` respectively to `glew\bin` and `glew\lib`.

###### glm

There is nothing to be done, this is a header-only library.

###### SFML

```
Source code:  C:/path/to/gourdin/external/SFML/
Build folder: C:/path/to/gourdin/external/SFML/build
```

Click on 'yes' when asked whether CMake should create the `build` directory.

Then copy the folder `SFML/build/lib` to `SFML\lib`.

#### 3. Configure your Code::Blocks project

Same as for building the dependencies, you need to generate the Code::Blocks project first with CMake.

```
Source code:  C:/path/to/gourdin/
Build folder: C:/path/to/gourdin/build
```

Click on 'yes' when asked whether CMake should create the `build` directory.

To be able to compile and run your project directly, you have to change some project parameters in Code::Blocks

Go in Project -> Properties, then on the tab "Build targets"

* Type: "GUI application"
* Output filename: "path/to/out.exe" (eg "bin/Release/out.exe")
* Execution working dir: "\.\."

Then you should be able to build and run gourdin :)
