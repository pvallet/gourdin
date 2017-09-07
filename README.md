# gourdin

This project is a 3D/2D engine developed under SDL and OpenGL for educational
purposes.

The structure of the map is taken from the map generator of this tutorial:
http://www-cs-students.stanford.edu/~amitp/game-programming/polygon-map-generation/

Any feedback be much appreciated. If that also gives you gamedesign ideas, tell me about it!

---

## Installation

On **Windows** you have two options:
* Download the [installer](https://github.com/pvallet/gourdin/releases/download/v0.2.0/gourdin-windows-setup_0.2.0.exe) to install gourdin on your computer. You will be able to run it through the windows menu and create a desktop icon if you want. If you have an error telling you that you don't have the permissions to run it, right ctrl-click on it and choose "Run as administrator".
* Download [gourdin-windows-folder.zip](https://github.com/pvallet/gourdin/releases/download/v0.2.0/gourdin-windows-folder_0.2.0.zip) that contains the directory with all the game info. You can unzip it and run it directly by double-clicking on `gourdin.exe`

On **Mac OS X** you should download [gourdin-mac.app.zip](https://github.com/pvallet/gourdin/releases/download/v0.2.0/gourdin-mac_0.2.0.app.zip). Then you only have to `ctrl-click` on it and choose open to allow executing from an unidentified developer (me).

On **Linux** there is no pre-built package, you have to build it from source.

On **Android** you can download the [apk](https://github.com/pvallet/gourdin/releases/download/v0.2.0/gourdin-android_0.2.0.apk), or download gourdin directly from the play store.

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
Then you will need the dependencies of the project. `./get_dependencies` is a small script that downloads locally the dependencies you might need. Under linux, the two valid entries are `glm` and `SDL2pp`.
```
sudo apt-get install libglm-dev libsdl2-dev libsdl2-image-dev
./get_dependencies SDL2pp
```

#### 2. Building the project

To build the project, simply run
```
mkdir build && cd build
cmake ..
make -j4
```

## OS X

#### 1. Homebrew

In order to easily install the dependencies, we recommend to use Homebrew, a package manager similar to apt-get in linux.
```
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```

#### 2. Dependencies

You then need CMake to generate your project and Clang (C++ compiler) to build it, and `wget` to retrieve your dependencies easily:
```
brew install cmake llvm wget
```
Finally install the dependencies of the project:
```
brew install glm sdl2 sdl2_image
./get_dependencies SDL2pp
```

`./get_dependencies` is a small script that downloads locally the dependencies you might need. Under OS X, the two valid entries are `glm` and `SDL2pp`.

#### 2. Building the project

In order to build the project, please refer to the section Linux -> 2. Building the project.

Please note that cmake might throw some errors about testing OpenMP. You can ignore them, the CMake config file is not up to date with the compiler Clang, that features OpenMP natively.

## Android (under linux)

#### 1. Requirements

- JDK and JRE 8 (`sudo apt-get install openjdk-8-jdk openjdk-8-jre`)
- Android SDK and NDK (with Android Build-tools 26.0.0 and Android Platform API 25, though these are configurable)
- ANDROID_HOME and ANDROID_NDK_HOME environment variables set (I did this in /etc/environment)

#### 2. Building the project

You need to first download all the local dependencies (SDL2, SDL2_image, SDL2pp, glm) and then you can build the app:
```
./get_dependencies
cd android
./gradlew assembleDebug
./gradlew installDebug
```

## Windows

#### 1. SDL

Download SDL2 development library corresponding to your compiler at the bottom of the following page:
https://www.libsdl.org/download-2.0.php

If you already use MinGW, make sure you have the version provided on the SDL website (next to the development library download).

Then download SDL2 image development library: https://www.libsdl.org/projects/SDL_image/

Unzip the two folders in `external\SDL2` and rename them so that your directory structure looks like this:

```
gourdin
+-- external
|   +-- SDL2
|   |   +-- SDL2
|   |   |   +-- bin
|   |   |   +-- include
|   |   |   +-- lib
|   |   +-- SDL2_image
|   |   |   +-- bin
|   |   |   +-- include
|   |   |   +-- lib
|   |   +-- Android.mk
|   +-- glew              (See below)
|   |   +-- bin
|   |   +-- lib
|   +-- glm               (See below)
|   |   +-- include
|   +-- libSDL2pp         (See below)
```

Make sure to copy the `bin`, `include` and `lib` corresponding to your compiler / platform to the roots of `SDL2` and `SDL2_image`.

#### 2. GLEW

http://glew.sourceforge.net/

The binaries provided on the official webpage are built for Visual Studio. If you use MinGW instead, you will need to build them from source. Before building glew, make sure that you have a compiler compatible with SDL (see previous section).

In order to do that, download the source and unzip it in `external`. You can build it with cmake, and then copy the folders `glew\build\bin` and `glew\build\lib` respectively to `glew\bin` and `glew\lib`.

#### 3. GLM and SDL2pp

GLM is a header-only library and thus does not need to be built. You can simply download it, unzip it and rename it in `external` as shown below.

https://github.com/g-truc/glm/releases

SDL2pp is a C++ wrapper for SDL and will be built statically while building gourdin via cmake. You also only have to unzip it and rename it in `external`.

https://github.com/libSDL2pp/libSDL2pp/releases


You can now create a new directory named `build` at the root of `gourdin`. And build the project with CMake.

---

## (Optional) Configure your Code::Blocks project

To be able to run your project, you have to change some project parameters in Code::Blocks

Go in Project -> Properties, then on the tab "Build targets"

* Type: "GUI application"
* Output filename: "path/to/out.exe" (eg "bin/Release/out.exe")
* Execution working dir: "\.\."
