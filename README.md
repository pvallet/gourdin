# gourdin

This project is a 3D/2D engine developped under SDL and OpenGL for educational
purposes.

The structure of the map is taken from the map generator of this tutorial:
http://www-cs-students.stanford.edu/~amitp/game-programming/polygon-map-generation/

Any feedback be much appreciated. If that also gives you gamedesign ideas, tell me about it!

---

## Installation

This version has been migrated from SFML to SDL and is thus not available yet as binaries.

All the previous versions are available here: https://github.com/pvallet/gourdin/releases/

---

## Building from source

The building is done with CMake to be cross-platform.

## Linux

#### 1. Dependencies

Firstly, install the dependencies for the building environment.
```
sudo apt-get install build-essential libgl1-mesa-dev libgomp1 cmake
```
Then you will need the dependencies of the project. `./get_dependencies` is a small script that dowloads locally the dependencies you might need. Under linux, the two valid entries are `glm` and `SDL2pp`.
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
---

## (Optional) Configure your Code::Blocks project

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
