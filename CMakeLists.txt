cmake_minimum_required(VERSION 3.22)

set(CMAKE_C_STANDARD                11)
set(CMAKE_C_STANDARD_REQUIRED       ON)
set(CMAKE_C_EXTENSIONS              ON)
set(CMAKE_CXX_STANDARD              20)
set(CMAKE_CXX_STANDARD_REQUIRED     ON)
set(CMAKE_CXX_EXTENSIONS            ON)
set(CMAKE_COLOR_DIAGNOSTICS         ON)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

if(WIN32)
	set(CMAKE_SYSTEM_NAME      Generic) # Windows workaround for arm-none-eabi-gcc
endif()

set_property(GLOBAL PROPERTY RULE_MESSAGES OFF)

project(BluePill VERSION 1.0.0 LANGUAGES C CXX ASM)

set(APP_DIR      ${PROJECT_SOURCE_DIR}/app)
set(LIBS_DIR     ${PROJECT_SOURCE_DIR}/libs)

# region HAL
set(HAL_INCLUDE_DIRS
    # Put here your include dirs, one in each line, relative to CMakeLists.txt file location
    ${PROJECT_SOURCE_DIR}/Core/Inc
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F1xx_HAL_Driver/Inc
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy
    ${PROJECT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F1xx/Include
    ${PROJECT_SOURCE_DIR}/Drivers/CMSIS/Include
    ${APP_DIR}
    #${LIBS_DIR}/ssd1309
)
# endregion

set(symbols_SYMB
    $<$<CONFIG:DEBUG>:"DEBUG">
    $<$<CONFIG:RELASE>:"NDEBUG">
    USE_HAL_DRIVER
    USE_FULL_LL_DRIVER
    STM32F103xB
    LOG_UTILS_PRINT_RTT
)

set(CPU_PARAMETERS
    -mcpu=cortex-m3
    -mthumb
)

set(ARM_NONE_EABI_FLAGS
    ${CPU_PARAMETERS}
    -Wall
    -Wextra
    -Wpedantic
    -Wno-unused-parameter
    -Wno-unused-variable
    -Wno-unused-function
    -fdata-sections
    -ffunction-sections
    -fdiagnostics-color=always
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-volatile>
    # Full debug configuration
    $<$<CONFIG:DEBUG>: -O0 -g3 -ggdb>
    # Release
    $<$<CONFIG:RELEASE>:-Os -s>
    # Release with DEBUG
    $<$<CONFIG:RelWithDebInfo>:-Os -g3 -ggdb>
)

# Set linker script
set(MCU_LINKER_SCRIPT ${PROJECT_SOURCE_DIR}/STM32F103C8Tx_FLASH.ld)

set(MCU_LINKER_OPTION
    -T${MCU_LINKER_SCRIPT}
    ${CPU_PARAMETERS}
    -Wl,-Map=${CMAKE_PROJECT_NAME}.map
    -specs=nano.specs
    -specs=nosys.specs
#    -u _printf_float                # STDIO float formatting support
    -Wl,--start-group
    -fno-exceptions
    -fno-rtti
    -Wl,--end-group
    -Wl,--print-memory-usage
)
# endregion

file(GLOB_RECURSE HAL_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/Core/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/Drivers/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/Core/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Drivers/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/*.s
)

file(GLOB_RECURSE APP_SOURCES
	${APP_DIR}/*.c
	${APP_DIR}/*.cpp
	${LIBS_DIR}/ssd1309/*.cpp
	${LIBS_DIR}/ssd1309/*.c
)

# Build lib
add_executable(${CMAKE_PROJECT_NAME} ${HAL_SOURCES} ${APP_SOURCES})

# Include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${HAL_INCLUDE_DIRS})

# Project symbols
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE ${symbols_SYMB})

# Compiler options
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE ${ARM_NONE_EABI_FLAGS})

# Linker options
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE ${MCU_LINKER_OPTION})

# # Convert output to hex and binary
add_custom_command(TARGET ${CMAKE_PROJECT_NAME}
	POST_BUILD
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMAND ${CMAKE_OBJCOPY} ARGS -O ihex -I elf32-littlearm ${CMAKE_PROJECT_NAME} ${CMAKE_PROJECT_NAME}.hex
	COMMAND ${CMAKE_OBJCOPY} ARGS -O binary -I elf32-littlearm ${CMAKE_PROJECT_NAME} ${CMAKE_PROJECT_NAME}.bin
)
