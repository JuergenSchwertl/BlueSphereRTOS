# Azure RTOS based sample for Azure Sphere

If you clone this repo, make sure you clone the submodules as well either using
>git clone https://github.com/JuergenSchwertl/BlueSphereRTOS.git BlueSphereRTOS **--recurse-submodules**
or after cloning this repo e.g. in Visual Studio Code open the TERMINAL tab and run
>git submodule update --init --recursive

This repo contains an exemplary source for an Azure RTOS project on Azure Sphere and step by step instructions on how to get there if you start from scratch.

##Prerequisites:
Please make sure to have a Windows 10 laptop capable of running Visual Studio 2019 or Visual Studio Code with the Azure Sphere SDK 
already installed and a free USB port available. 
* Visual Studio 2019 Download: https://visualstudio.microsoft.com/vs/ <br>
In Visual Studio please go to *Extensions* -> *Manage Extensions* and download the **Azure Sphere** Extension for Visual Studio. 
This may also download dependencies like C/C++ and the Linux development evironment for Visual Studio so it may take a little while.


* alternatively Visual Studio Code Download: https://code.visualstudio.com/ <br>
In Visual Studio Code, please also download the **Azure Sphere** extension (which will also download the **C/C++** and **CMake Tools** Extensions). 
Please also follow the instructions on the Extension page to configure your settings.json e.g. via File->Preferences->Settings ! 
>**Note**: If you run Visual Studio 2019 and Visual Studio Code side by side, you actually don't need to install the CMake tools, Ninja and an ARM Gnu toolchain seperately 
>but can refer to the VS2019 versions in settings.json alike:
>```json
>    "AzureSphere.SdkPath": "C:\\Program Files (x86)\\Microsoft Azure Sphere SDK\\",
>    "AzureSphere.ArmGnuPath": "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\Linux\\gcc_arm",
>    "cmake.cmakePath": "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\Common7\\IDE\\CommonExtensions\\Microsoft\\CMake\\CMake\\bin\\cmake.exe",
>    "cmake.configureSettings": {
>        "CMAKE_MAKE_PROGRAM" : "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\Common7\\IDE\\CommonExtensions\\Microsoft\\CMake\\Ninja\\ninja.exe"
>    }
>```
>The `"AzureSphere.ArmGnuPath"` needs to point to your ARM toolchain (compilers are required to be located in the ...\bin\... subdirectory underneath.
>In my example I'm referring to the toolchain installed with Visual Studio 2019 Community edition. For Enterprise edition this would be ...\2019\Enterprise\...



