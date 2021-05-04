# Azure RTOS based sample for Azure Sphere

If you clone this repo, make sure you clone the submodules as well either using
>git clone https://github.com/JuergenSchwertl/BlueSphereRTOS.git BlueSphereRTOS **--recurse-submodules**
or after cloning this repo e.g. in Visual Studio Code open the TERMINAL tab and run
>git submodule update --init --recursive

This repo contains an exemplary source for an Azure RTOS project on Azure Sphere and step by step instructions on how to get there if you start from scratch.

## Prerequisites:
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

### Azure RTOS::threadX, OS_HAL, M-HAL and why the Azure Sphere SDK doesn't help here?
If you look at the Azure Sphere SDK and what's needed to compile a high level app, you'll see that the SDK uses the **ARM-POKY-LINUX-MUSLEABI** toolchain. If that doesn't tell you much:
we're running the compiler toolchain on your x86 Windows/Linux development machine but need to cross-compile for the POSIX environment on the MT3620:
in that case for the ARM Cortex A7 core of the MT3620; thats the **ARM-** bit.
The Azure Sphere Operating System is based on an embedded YOCTO Linux (that's the **POKY-LINUX**-bit) and the C stdlib is a MIT licensed implementation for Unix SysCall (**MUSL**)
embedded application binary interfaces (that's the **EABI**-bit).

However, we want to build a realtime-capable application either running bare metal on any of the two the ARM Cortex M4 cores and here in particular bring-your-own-OS: Azure RTOS::threadX ==> 
so there is no high-level operating system taking care of memory allocation ala malloc() or helping with debug output to *stdout* or alike. So what's next?

What we need is a toolchain that allows cross compilation to ARM without a target OS: **ARM-NONE-EABI**.
 
Visual Studio fortunately has an ARM GNU toolchain available for you: just check that you have *"Linux Development with C++"* 
selected and therein *"Embedded and IoT development tools"*. This should have the toolchain files installed in
*C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Linux\gcc_arm* (I'm running VS and VSC on the same machine and therefore have settings.json in VSC refer to the VS-Toolchain 
for the "AzureSphere.ArmGnuPath"). 

If you run Visual Studio Code only, you might want to peek at the extension [GNU Arm embedded toolchain for Windows](https://marketplace.visualstudio.com/items?itemName=metalcode-eu.windows-arm-none-eabi)
or download the toolchain directly from [armDeveloper](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads). 

## Debugging considerations
The GDB debugger needs a little while to connect to the app running on the M4 core and by the time if is finally setup, your app already has started and run
for a considerable amount of time. It's therefore a bit tricky to debug the application start unless you use a little workaround: 
add a loop waiting for a local variable to change and set a break point inside the loop. The app will start and wait, spinning cycles in the loop until you 
break and change the variable in the debugger (e.g. set `nWait = 0` in the debugger to continue). 
```C
    volatile int iWait = 1;
    while (iWait)
    {
        ;
    }
```

Since we may not have any operating system available and thus also no _STDOUT_ or *Log_Debug()* on the M4 cores we want to use a UART to output our debugging messages.
The MediaTek BSP *MT3620_M4_BSP* comes with a [printf_()](./mt3620_m4_software/MT3620_M4_BSP/printf/printf.c#L862) function that can be redirected by implementing a hook for the underlying 
character output function `void _putchar(char)`. 
You'll see the hook provided in [void _putchar(char character) in rtcoremain.cpp](./startup/rtcoremain.cpp#L38), sending the output to an UART and the hardware initialisation in the 
RtCoreMain() function.

In our case this is output to the hardwired DEBUG UART of the M4 core (`OS_HAL_UART_PORT0`), our app runs on. 
On the Seeed Dev Kit this could be IO0_TX (Header 3, Pin 6) or IO1_TX respectively (Header 3, Pin 8). Just connect an arbitrary USB->UART converter and let PuTTY or any HyperTerm-like
application show the output. 

>**Note:** PORT0 does not need to be added to the _appmanifest.json_ and is always available. If you run two M4 apps, you may not necessarily know which app runs on which core, 
>so it may be a bit try and error to find the correct pin. You can also choose to use one of the available other ISUs (e.g. `OS_HAL_UART_ISU0`) but then you 
>need to make sure you enable the ISU also in the _appmanifest.json_ in the  _"Uart"_ list under _"Capabilities"_ .

As UART initialisation and output is hardware dependent and you likely don't want to learn the hundreds of register addresses and bit-combos, 
you will want to use a Hardware Abstraction Layer, namely the *MediaTek OS_HAL*.

## Understanding the MTK OS_HAL, MHAL, MDL and BSP_

For some reason MediaTek chose to put the OS_HAL under samples so you find it under [MT3620_M4_Sample_Code/OS_HAL](./mt3620_m4_software/blob/d9ac51feac9843af769b3d9783bfd8b9407e79bc/MT3620_M4_Sample_Code/OS_HAL) 
but it misses a CMakeLists.txt. You therefore need to add the files under the ./src-directory manually to your main CMakeLists.txt .
You could elect to add individual hardware related sources (i.e. os_hal_gpio.c ) into your project or build them all together as a library and let the linker decide
what functions to bind from the lib. In my example I chose to build it as a lib as you can see in the CMakeLists.txt under
```CMake
##################################################################
#  MTK_OS_HAL library section
##################################################################
```

Let's traverse the [MTK GitHub repo](https://github.com/MediaTek-Labs/mt3620_m4_software/) on the example of the uart initialisation function: `mtk_os_hal_uart_ctlr_init(OS_HAL_UART_PORT0)`.
- In RTCoreMain we call the OS_HAL function [mtk_os_hal_uart_ctlr_init() in os_hal_uart.c](./mt3620_m4_software/blob/d9ac51feac9843af769b3d9783bfd8b9407e79bc/MT3620_M4_Sample_Code/OS_HAL/src/os_hal_uart.c#L131)
- which in turn calls the *MT3620_M4_Driver* - MHAL function [mtk_mhal_uart_hw_init() in mhal_uart.c](./mt3620_m4_software/blob/d9ac51feac9843af769b3d9783bfd8b9407e79bc/MT3620_M4_Driver/MHAL/src/mhal_uart.c#L77)
- which in turn calls the *MT3620_M4_Driver* - HDL function [mtk_hdl_uart_init() in mhdl_uart.c](./mt3620_m4_software/blob/d9ac51feac9843af769b3d9783bfd8b9407e79bc/MT3620_M4_Driver/HDL/src/hdl_uart.c#L75)
- that finally sets the bare metal registers (*for some weird reason that is done one level up on MHAL in mhal_osai.c again, but anyway?*)

The *MT3620_M4_Driver* directory has a [MT3620_M4_Driver/CMakeLists.txt](./mt3620_m4_software/blob/d9ac51feac9843af769b3d9783bfd8b9407e79bc/MT3620_M4_Driver/CMakeLists.txt), so we can add that in our main CMakeLists.txt as a subdirectory:
```CMake
    # add MediaTek MT3620 M4 driver library 
    add_subdirectory(mt3620_m4_software/MT3620_M4_Driver)
``` 
It creates the library *MT3620_M4_Diver* that we need to add in our linker definition
```CMake
    target_link_libraries (${PROJECT_NAME}  MT3620_M4_Driver azrtos::threadx)
```

The *MT3620_M4_Driver* does also build the underlying *MT3620_M4_BSP* using it's [MT3620_M4_BSP/CMakeLists.txt](./mt3620_m4_software/blob/d9ac51feac9843af769b3d9783bfd8b9407e79bc/MT3620_M4_BSP/CMakeLists.txt) and already links it to the driver lib.


But let's now have a look on the startup sequence and what it takes to also enable C++ on the M4 core:

## MT3620 startup sequence

The very first startup code (a.k.a Reset vector) is part of the [MT3620_M4_BSP](./mt3620_m4_software/blob/d9ac51feac9843af769b3d9783bfd8b9407e79bc/MT3620_M4_BSP/CMakeLists.txt) , 
and implemented in Assembly in [startup_mt3620.s](./mt3620_m4_software/blob/d9ac51feac9843af769b3d9783bfd8b9407e79bc/MT3620_M4_BSP/mt3620/src/startup_mt3620.s) cleaning up the memory 
and immediately branches into our [_Noreturn void RTCoreMain(void)](./startup/rtcoremain.cpp#L38).

Here we initialize the Interrupt Vector Table
```C
    // Init Vector Table
    NVIC_SetupVectorTable();
```

Do our little trick to wait for the Debugger to have a chance to connect to the M4 core and then initialize the DEBUG UART

```C
    // Init UART: defaults to uses 115200,8N1
    mtk_os_hal_uart_ctlr_init(nUartPortNumber);
    
    printf("UART Initialized\n");
```

C++ needs some additional handiwork to call global constructors

```C
#ifdef __cplusplus
    // Call global constructors
    for (InitFunc* func = &__init_array_start; func < &__init_array_end; ++func) (*func)();
#endif
```

Once that startup is done, we'll continue in main() and start Azure RTOS

## Azure RTOS startup sequence

The main() function in [main.cpp](./BlueSphereRTOS/src/main.cpp#113) is rather short
```C
int main(void)
{
    /* Enter the ThreadX kernel.  */
    tx_kernel_enter();

    return STATUS_OK;
}
```  

As you can see it just sets up the kernel and will actually never return. Implicitly it calls into low level Assembly code setting up the 100Hz SysTick
timer interrupt and service routine in [tx_initialize_low_level.S](./startup/tx_initialize_low_level.S) and then calls into:
```C
void tx_application_define(void* first_unused_memory)
```
Here you can initialize hardware as appropriate and start your application threads...