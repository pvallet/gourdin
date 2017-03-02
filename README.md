# gourdin

This project is a 3D/2D engine developped under SFML and OpenGL for educational
purposes.

For more details about its development, please refer to report.pdf (coming soon).

## Installation

The installation is done with CMake to be cross-platform.

## Linux

#### 1. Dependencies

Firstly, install the dependencies for the building environment.
```
sudo apt-get install build-essential libgl1-mesa-dev libgomp1 cmake
```
Then you will need the dependencies of the project. Prefer using the ones from the repositories as building them from the source takes a while.
```
libsfml-dev libglew-dev libglm-dev
```
You might need to build these dependencies locally, for example if your repository does not have the right version (e.g. SFML < 2.4.1). In order to do that, run the script
```
./get_dependencies
```
to get all of them, or specify the libraries you want to build locally
```
./get_dependencies SFML
```

The entries are `glm`, `SFML` and `glew`.

#### 2. Building the project

To build the project, simply run
```
mkdir build && cd build
cmake ..
make -j4
```

To run gourdin, go the root of the project (with `cd ..`) and then run the executable:
```
./build/bin/Release/out
```

Be sure to pick the right path, for example if you chose a debug build.

If you want to configure Code::Blocks in order to use its debugger, please refer to the section

Windows -> 3.Configure your Code::Blocks project

## Windows

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
