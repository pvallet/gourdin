# gourdin

This project is a 3D/2D engine developped under SFML and OpenGL for educational
purposes.

For more details about its development, please refer to report.pdf (coming soon).

## Installation

The installation is done with CMake to be cross-platform.

### Linux

Install dependencies

```
sudo apt-get install build-essential libgl1-mesa-dev libsfml-dev libglew-dev libgomp1 libflann-dev libglm-dev
```

If your repository has a version of SFML lower than 2.4.1, you should compile the library by yourself to avoid a bug of the mouse coordinates.

Then build the project
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

### Windows

All the steps will be explained as if the user was using Code::Blocks. It is easy to adapt the tutorial to other IDEs.

#### 1. OpenMP

The first step is to use a compiler that supports OpenMP. It will also be used to compile FLANN.

TDM-GCC does it pretty well on windows. Here is a tutorial on how to install it on windows. During the installation of TDM-GCC, don't forget to check the OpenMP box in the gcc menu when choosing your components.

http://wiki.codeblocks.org/index.php/MinGW_installation

#### 2. Dependencies

You have then to retrieve all the dependencies on github, and compile them with your compiler, so as to make them compatible with gourdin.

##### Dependencies

SFML 2.4.1 or higher, FLANN, GLEW and GLM

https://github.com/SFML/SFML

https://github.com/mariusmuja/flann

https://github.com/nigels-com/glew

https://github.com/g-truc/glm

Be sure to get the code from a release. GLEW for example only provides the includes in the last release.

Coming soon - retrieve them easily with git submodules.

####3. Configure your Code::Blocks project

To be able to compile and run your project directly, you have to change some project parameters in Code::Blocks

Go in Project -> Properties, then on the tab "Build targets"

* Type: "Console application"
* Output filename: "path/to/out" (eg "bin/Release/out")
* Execution working dir: "\.\."
