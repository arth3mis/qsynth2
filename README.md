# QSynthi2
Quantum Synthesiser from Schrödinger's Equation - Advanced

## About the project

This project aims to explore the sonification (data→sound) of 2D simulations,
with the Schrödinger Equation as a starting point.

The original project (with our Paper) can be found at http://qsynthi.com 
and on [GitHub](https://github.com/arth3mis/qsynthi).

---

## Plugin Usage (VST3, AU)

Go to Releases and download the file that is suitable for your
OS and DAW.

If you need a version that is not included there, or you want to
get instant updates during the development process, 
you should [build](#custom-build-setup) the plugin yourself.

---

## Custom Build Setup

This project can be easily built using CMake.
The following tools must be installed on your system:
- CMake Version 3.22 or higher
- C++ compiler for C++ 20, e.g. g++, clang, MinGW
- There may be additional dependencies depending on your system
  - Linux: please refer to [this list](https://github.com/juce-framework/JUCE/blob/master/docs/Linux%20Dependencies.md).

Clone the repository and open it in your IDE.

When your build has succeeded, you will find the VST3 and AU Plugin files here (relative to the project directory):
``build/plugin/AudioPlugin_artefacts/Release/VST3/``
``build/plugin/AudioPlugin_artefacts/Release/AU/``

If errors occur during the setup, you can retry by deleting the build folder.
Before you give up, try the [manual method](#manual-build), it is sometimes simpler than IDE setups
and shows errors more clearly.
There are also many forum posts about C++/CMake/Juce builds. Warning: Juce offers
another way of building ("Projucer"), this does **not** apply to this project!

---

### VS Code
Please install the CMake extension (twxs.cmake) inside Visual Studio Code.

Using the CMake tab, select 

Visual Studio Code should be able to detect the project and
offer you to configure it. If it doesn't, make sure that the  is installed.
You can also right-click the file ``CMakeLists.txt`` in the project root directory and select "Configure All Projects".

In the CMake tab of VS Code, you will now find the "Project Outline" containing
the buildable targets. Select ``AudioPlugin_All``

### CLion

CLion should detect everything automatically.

### Microsoft Visual Studio

(not tested yet)

### Manual build
Run these commands in the root directory of the project:
```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
```
Then run the final build command.
- Edit the number after ``-j`` to select the number of CPU cores used
- There are different options for ``--target`` (if you only want to build VST3, for example). 
  To list all available, execute the command below with ``--target help``
```
cmake --build . --target AudioPlugin_All -j 8
```

---

## License
See [LICENSE](LICENSE).
