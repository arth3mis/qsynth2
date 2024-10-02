# QSynth2
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

This project can be built using CMake.
The following tools must be installed on your system:
- CMake Version 3.22 or higher
- C++ compiler for C++ 20, e.g. g++, clang, MinGW
- There may be additional dependencies depending on your system
  - Linux: please refer to [this list](https://github.com/juce-framework/JUCE/blob/master/docs/Linux%20Dependencies.md).

Clone the repository and open it in your IDE.

When your build has succeeded, you will find the VST3 and AU Plugin files here (relative to the project directory):  
``build/plugin/AudioPlugin_artefacts/Release/VST3/`` (on Windows, the actual VST3 file is further nested in ``.../QSynth2.vst3/Contents/x86_64-win/``)  
``build/plugin/AudioPlugin_artefacts/Release/AU/``

If errors occur during the setup, you can retry by deleting the build folder.
Before you give up, try the [manual method](#manual-build), it is sometimes simpler than IDE setups
and shows errors more clearly.
There are also many forum posts about C++/CMake/Juce builds. Warning: Juce offers
another way of building ("Projucer"), this does **not** apply to this project!

---

### VS Code

Please install the CMake extension (twxs.cmake) inside Visual Studio Code.

Visual Studio Code should be able to detect the project and offer you to configure it.
You can also right-click the file ``CMakeLists.txt`` in the project root directory and select "Configure All Projects".

In the CMake tab on the left side of VS Code, you will now find the "Project Status". Under "Configure", 
set your CMake Toolkit if none is selected, and change the build variant from ``Debug`` to ``Release``.
In the same tab, you find the "Project Outline" containing the buildable targets. 
Select ``AudioPlugin_All`` (grouped under "AudioPlugin") as the default build target, or build it via the button next to it or by right-clicking.

If a Debug build was running and you change to Release, you must delete the ``build`` folder in the main directory before rebuilding.

### CLion

CLion should detect everything automatically, load the CMake project and add the configurations.
If not, please search for "CLion reload CMake project" online.
Select ``AudioPlugin_All`` in the dropdown menu at the top and click the Build button next to the menu.


### Microsoft Visual Studio

If you open the project with Visual Studio 2022, it will show a CMake overview page
and add the file ``CMakeSettings.json``. Go to this file and add the configuration "x64-Release".
Click on this configuration and set the **Configuration type** from "RelWithDebInfo" to "Release".
Save the file (Ctrl+S) to reload the CMake project.

Select the "x64-Release" configuration in the dropdown menu to the left of the start button.
Then, at the very top, click on Build→Install. The plugins will be created in 
``out/build/x64-Release/plugin/AudioPlugin_artefacts/Release/`` 
(note that the individual files are not directly in this folder, but further down the path).


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
If you use the CMake executable by Visual Studio, you need to pass
the configuration with the build command:
```
cmake --build . --target AudioPlugin_All -j 8 --config Release
```


---

## License
See [LICENSE](LICENSE).
