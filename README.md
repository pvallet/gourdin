# gourdin

[![Gourdin](http://img.youtube.com/vi/DIJxMQXnvy0/0.jpg)](http://www.youtube.com/watch?v=DIJxMQXnvy0 "Gourdin")

Click on the image to go to a YouTube video presenting the project.

This project is a 3D/2D engine developed under SDL and OpenGL for educational
purposes.

The structure of the map is taken from the map generator of this tutorial:
http://www-cs-students.stanford.edu/~amitp/game-programming/polygon-map-generation/

Any feedback is much appreciated. If that also gives you gamedesign ideas, tell me about it!

---

## Building from source

The buildsystem is CMake to be cross-platform. The script takes care of downloading the dependencies of the project and building them from source.

## Linux

Firstly, install the dependencies for the building environment.
```
sudo apt-get install build-essential libgl1-mesa-dev libgomp1 cmake
```

To build the project, simply run
```
mkdir build && cd build
cmake ..
make -j4
```

## OS X

In order to easily install the dependencies, Homebrew is a good option. (Similar to apt-get in linux).
```
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```
You then need CMake to generate your project and Clang (C++ compiler) to build it:
```
brew install cmake llvm
```
Building the project is the same as on linux:
```
mkdir build && cd build
cmake ..
make -j4
```

Please note that cmake might throw some errors about testing OpenMP. You can ignore them, the CMake config file is not up to date with the compiler Clang, that features OpenMP natively.

## Android (under linux)

The project is also compatible with Android Studio.

#### 1. Requirements

- JDK and JRE 8 (`sudo apt-get install openjdk-8-jdk openjdk-8-jre`)
- Android SDK and NDK (with Android Build-tools 28.0.3 and Android Platform API 28, though these are configurable)
- ANDROID_HOME and ANDROID_NDK_HOME environment variables set (I did this in /etc/environment)

#### 2. Building the project

```
cd android
./gradlew assembleDebug
./gradlew installDebug
```

## Windows

Download and install CMake from its website: https://cmake.org/download/
You can now create a new directory named `build` at the root of `gourdin`. Running CMake will generate the project files for the EDI you use. You can then open the solution files and start the build.
The VS solution should be configured automatically, while a Code::Blocks project needs a bit more attention (see next section).

In VS, you choose the build type within the solution, while it would be done with CMake for Code::Blocks

---

## (Optional) Configure your Code::Blocks project

To be able to run your project, you have to change some project parameters in Code::Blocks

Go in Project -> Properties, then on the tab "Build targets"

* Type: "GUI application"
* Output filename: "path/to/out.exe" (eg "bin/Release/out.exe")
* Execution working dir: "\.\."
