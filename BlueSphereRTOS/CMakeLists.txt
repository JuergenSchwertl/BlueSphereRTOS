#  Copyright (c) Microsoft Corporation. All rights reserved.
#  Licensed under the MIT License.

cmake_minimum_required (VERSION 3.11)
project (BlueSphereRTOS C CXX ASM)

azsphere_configure_tools(TOOLS_REVISION "21.04")

# set global options 
set(CMAKE_ASM_FLAGS "-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Wall")
add_link_options(-specs=nano.specs -specs=nosys.specs)
set(CMAKE_C_FLAGS "-mcpu=cortex-m4 -fno-common -mthumb  -mfloat-abi=hard -mfpu=fpv4-sp-d16")
set(CMAKE_CXX_FLAGS "-std=gnu++14 -fno-common -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16")

message("\r\n**** ${PROJECT_NAME} section\r\n")

# build your project executable
add_executable (${PROJECT_NAME}
    "startup/rtcoremain.cpp" 
    "startup/tx_initialize_low_level.S" 
    "src/Logger.cpp" 
    "src/Led.cpp" 
    "src/BlinkingLed.cpp"
    "src/main.cpp" 
)

target_include_directories(${PROJECT_NAME} PUBLIC "./inc")

##################################################################
#  MT3620_M4_Driver library section
##################################################################
message("\r\n**** MT3620_M4_Driver library section\r\n")

# set root directory for MTK submodule
set(MTK_ROOT "../mt3620_m4_software")

# set root directory for M4 driver & BSP
set(MTK_MT3620_M4_DRIVER_ROOT "${MTK_ROOT}/MT3620_M4_Driver")
set(MTK_MT3620_M4_BSP_ROOT "${MTK_ROOT}/MT3620_M4_BSP")

set( MTK_MT3620_DRIVER_INCLUDES
    "${MTK_MT3620_M4_DRIVER_ROOT}/MHAL/inc"
    "${MTK_MT3620_M4_DRIVER_ROOT}/HDL/inc"
    "${MTK_MT3620_M4_BSP_ROOT}/CMSIS/include"
    "${MTK_MT3620_M4_BSP_ROOT}/mt3620/inc"
    "${MTK_MT3620_M4_BSP_ROOT}/printf"
)
# add compiler definition for MT3620_M4_Driver library 
add_compile_definitions(OSAI_AZURE_RTOS)
#add_compile_definitions(OSAI_ENABLE_DMA)
# If you want to place CODE_REGION in FLASH instead of TCM, please enable this definition:
# add_compile_definitions(M4_ENABLE_XIP_FLASH)

# build MediaTek MT3620 M4 driver library, BSP and initial startup code into library libMT3620_M4_Driver.a
add_subdirectory(  ${MTK_MT3620_M4_DRIVER_ROOT} "./lib/MT3620_M4_Driver")


##################################################################
#  MTK_OS_HAL library section
##################################################################
message("\r\n**** MTK_OS_HAL library section\r\n")

set(MTK_OS_HAL "os_hal")

set(MTK_OS_HAL_ROOT "${MTK_ROOT}/MT3620_M4_Sample_Code/OS_HAL")
set(MTK_OS_HAL_SRC "${MTK_OS_HAL_ROOT}/src")

set(MTK_OS_HAL_FILES
    "${MTK_OS_HAL_SRC}/os_hal_adc.c"
    "${MTK_OS_HAL_SRC}/os_hal_dma.c"
    "${MTK_OS_HAL_SRC}/os_hal_eint.c"
    "${MTK_OS_HAL_SRC}/os_hal_gpio.c"
    "${MTK_OS_HAL_SRC}/os_hal_gpioif.c"
    "${MTK_OS_HAL_SRC}/os_hal_gpt.c"
    "${MTK_OS_HAL_SRC}/os_hal_i2c.c"
    "${MTK_OS_HAL_SRC}/os_hal_i2s.c"
    "${MTK_OS_HAL_SRC}/os_hal_lp.c"
    "${MTK_OS_HAL_SRC}/os_hal_mbox.c"
#    "${MTK_OS_HAL_SRC}/os_hal_mbox_shared_mem.c"  # needs "volatile u8 blockFifoSema;" declared in your source (e.g. main.c)
    "${MTK_OS_HAL_SRC}/os_hal_pwm.c"
    "${MTK_OS_HAL_SRC}/os_hal_spim.c"
    "${MTK_OS_HAL_SRC}/os_hal_uart.c"
    "${MTK_OS_HAL_SRC}/os_hal_wdt.c" 
)

#set(CMAKE_C_FLAGS "-mcpu=cortex-m4 -fno-common -mthumb  -mfloat-abi=hard -mfpu=fpv4-sp-d16")

# build the OS HAL library
add_library(${MTK_OS_HAL} ${MTK_OS_HAL_FILES})

# add the OS_HAL, M4 Driver & BSP header directories
target_include_directories(${MTK_OS_HAL} PUBLIC
    "${MTK_OS_HAL_ROOT}/inc"
    ${MTK_MT3620_DRIVER_INCLUDES}
)

target_include_directories(${PROJECT_NAME} PUBLIC
    "${MTK_OS_HAL_ROOT}/inc"
    ${MTK_MT3620_DRIVER_INCLUDES}
)

##################################################################
#  Azure RTOS:threadX library section
##################################################################

message("\r\n**** Azure RTOS:threadX library section\r\n")
set(THREADX_ROOT "../threadx")

# build Azure-RTOS ( threadX ) kernel for M4 core using Gnu toolchain into library libthreadx.a
set(THREADX_ARCH "cortex_m4")
set(THREADX_TOOLCHAIN "gnu")
set(VFP_FLAGS "-mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16")
#add_compile_definitions(TX_ENABLE_FPU_SUPPORT)
add_subdirectory( ${THREADX_ROOT} "./lib/threadX")


                            

##################################################################
#  linker section
##################################################################


#list all required libraries
target_link_libraries (${PROJECT_NAME} ${MTK_OS_HAL} MT3620_M4_Driver azrtos::threadx)

#set dependency to linker script
set_target_properties (${PROJECT_NAME} PROPERTIES LINK_DEPENDS "${CMAKE_SOURCE_DIR}/linker.ld")

# post-build command to create Azure Sphere .imagepackage file
azsphere_target_add_image_package(${PROJECT_NAME})
