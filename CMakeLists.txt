#  Copyright (c) Microsoft Corporation. All rights reserved.
#  Licensed under the MIT License.

cmake_minimum_required (VERSION 3.11)

project (BlueSphereRTOS C ASM)

azsphere_configure_tools(TOOLS_REVISION "21.01")

set(CMAKE_ASM_FLAGS "-mcpu=cortex-m4")
add_compile_definitions(OSAI_AZURE_RTOS)

set(MTK_OS_HAL_FILES
    mt3620_m4_software/MT3620_M4_Sample_Code/OS_HAL/src/os_hal_adc.c
    mt3620_m4_software/MT3620_M4_Sample_Code/OS_HAL/src/os_hal_dma.c
    mt3620_m4_software/MT3620_M4_Sample_Code/OS_HAL/src/os_hal_eint.c
    mt3620_m4_software/MT3620_M4_Sample_Code/OS_HAL/src/os_hal_gpio.c
    mt3620_m4_software/MT3620_M4_Sample_Code/OS_HAL/src/os_hal_gpioif.c
    mt3620_m4_software/MT3620_M4_Sample_Code/OS_HAL/src/os_hal_gpt.c
    mt3620_m4_software/MT3620_M4_Sample_Code/OS_HAL/src/os_hal_i2c.c
    mt3620_m4_software/MT3620_M4_Sample_Code/OS_HAL/src/os_hal_i2s.c
    mt3620_m4_software/MT3620_M4_Sample_Code/OS_HAL/src/os_hal_lp.c
    mt3620_m4_software/MT3620_M4_Sample_Code/OS_HAL/src/os_hal_mbox.c
    mt3620_m4_software/MT3620_M4_Sample_Code/OS_HAL/src/os_hal_pwm.c
    mt3620_m4_software/MT3620_M4_Sample_Code/OS_HAL/src/os_hal_spim.c
    mt3620_m4_software/MT3620_M4_Sample_Code/OS_HAL/src/os_hal_uart.c
    mt3620_m4_software/MT3620_M4_Sample_Code/OS_HAL/src/os_hal_wdt.c )

add_link_options(-specs=nano.specs -specs=nosys.specs)



# Create executable
add_executable (${PROJECT_NAME} ${MTK_OS_HAL_FILES} rtcoremain.c main.c tx_initialize_low_level.S)

target_include_directories(${PROJECT_NAME} PUBLIC mt3620_m4_software/MT3620_M4_Sample_Code/OS_HAL/inc)
target_include_directories(${PROJECT_NAME} PUBLIC
                            mt3620_m4_software/MT3620_M4_Driver/MHAL/inc
                            mt3620_m4_software/MT3620_M4_Driver/HDL/inc
                            mt3620_m4_software/MT3620_M4_BSP/CMSIS/include
                            mt3620_m4_software/MT3620_M4_BSP/mt3620/inc
                            mt3620_m4_software/MT3620_M4_BSP/printf)

# add MediaTek MT3620 M4 driver library 
add_subdirectory(mt3620_m4_software/MT3620_M4_Driver)

# add Azure-RTOS ( threadX ) kernel library
set(THREADX_ARCH "cortex_m4")
set(THREADX_TOOLCHAIN "gnu")
add_subdirectory(threadx)

target_link_libraries (${PROJECT_NAME}  MT3620_M4_Driver azrtos::threadx)
set_target_properties (${PROJECT_NAME} PROPERTIES LINK_DEPENDS ${CMAKE_SOURCE_DIR}/linker.ld)

# post-build command to create Azure Sphere image package
azsphere_target_add_image_package(${PROJECT_NAME})
