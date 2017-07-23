# gourdin

This project is a 3D/2D engine developped under SFML and OpenGL for educational
purposes.

---

Two modes are featured in gourdin, a Sandbox mode and a Game mode.

The **Sandbox** mode is made to be able to see the whole structure of the island and navigate through the different biomes easily. This is still a work in progress so there is still obvious work to be done on texture transitions and forests structure.

There are some antilope and deer herds scattered over the island, which appear only on certain biomes. The antilopes appear only on the biome with small plants with big leaves (making the environment more consistent is on the TO-DO list). The deers are easier to find.

As a remnant from the earlier development stages, you can hunt them by spawning predators with the right click in the adequate biomes, and control them as in a classic RTS. Up to you to find where you can spawn wolves, lions and leopards ;) You can launch a hunt session where your number of kills will be recorded. In this mode you cannot spawn predators too close to the deers.

Any feedback on the structure of the island and the environment is much appreciated (the controls of the predators is not the main purpose of this mode).

The **Game** mode is made to show how would be the controls and the camera for a game using this engine. In this mode you control a small tribe that you can move around, switching focus among characters. There are two camera types, a 'God' camera and a point-of-view one.

You can move around using the mouse or the keyboard. The main purpose is to see if this type of controls would be suitable for portable devices.

Any feedback on the global ergonomy of the controls and camera would be much appreciated. If that also gives you gamedesign ideas, tell me about it!

---

## Installation

On **Windows** you have two options:
* Download the [installer](https://github.com/pvallet/gourdin/releases/download/v0.1.0/gourdin-windows-setup_0.1.0.exe) to install gourdin on your computer. You will be able to run it through the windows menu and create a desktop icon if you want. If you have an error telling you that you don't have the permissions to run it, right ctrl-click on it and choose "Run as administrator".
* Download [gourdin-windows-folder.zip](https://github.com/pvallet/gourdin/releases/download/v0.1.0/gourdin-windows-folder_0.1.0.zip) that contains the directory with all the game info. You can unzip it and run it directly by double-clicking on `gourdin.exe`

On **Mac OS** you should download [gourdin-mac.app.zip](https://github.com/pvallet/gourdin/releases/download/v0.1.0-rc/gourdin-mac.app.zip). Then you only have to `ctrl-click` on it and choose open to allow executing from an unidentifed developer (me).

On **Linux** there is no pre-built package, you have to build it from source.

All the files are available here: https://github.com/pvallet/gourdin/releases/

---

## Usage

The controls are displayed in the main screen in gourdin under Linux and Windows. However, Mac OS requires a greater OpenGL version that is not compatible with SFML, hence the interface is absent.

This is why we also provide the controls here.

#### 1. Sandbox mode
The controls of the sandbox mode are pretty much the same as a RTS game (see Populous: The Beginning).

```
Esc: Quit game
M: Switch to Sandbox mode
P: Pause
Left-Right / Q-D:  Rotate camera
Up-Down    / Z-S:  Go forwards/backwards
A/Return: Select all lions
B: Launch benchmark
E: Switch scroll speed to fast/slow
H: Start/stop hunting session
L: Hide/Display log
W: Switch to wireframe display

Click on the minimap to jump there
Right-click to make a lion appear
Select it with the left mouse button
Move it around with the right button
Lshift/Rshift:  Make it run
Delete: Kill selected lion
```

#### 2. Game mode

###### God camera
```
Esc: Quit game
M: Switch to Sandbox mode
P: Pause
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

Then you should be able to build and run gourdin :)
