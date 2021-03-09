# Author: Jack Greenberg <j@jackgreenberg.co>

# Define minimum CMake version (required)
cmake_minimum_required(VERSION 3.2)

# Define the project name
project(btldr
    VERSION 0.0.1
    LANGUAGES C
)

# Set the CMake Toolchain (arm-none-eabi)
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/toolchain.cmake")
include("${CMAKE_TOOLCHAIN_FILE}")
set(TOOLCHAIN_DIR /usr/lib/avr)

# Check to make sure the build architecture is arm-none-eabi
if (NOT CMAKE_SYSTEM_PROCESSOR STREQUAL "avr")
    message(FATAL_ERROR "Invalid CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")
else()
    message(STATUS "Using CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")
endif()
set(COMPILER_OPTIMISATION "-O3")

set(CMAKE_C_FLAGS " \
    -Wall -Werror \
    -Wl, -Map=${PROJECT_NAME}.map --gc-sections -lm -lc \
")

# # Include the CMakeLists.txt for libopencm3 (effectively jumps to the file)
# include(cmake/libopencm3.cmake)
# include(cmake/freertos.cmake)

# set(STM32_COMPILER_OPTIONS "${STM32_ASM_OPTIONS} \
#     -ffunction-sections \
#     -fdata-sections \
#     -mfloat-abi=soft \
#     -fmessage-length=0 \
#     -Wall -Werror \
#     -Wl,-Map=output.map -Wl,--gc-sections \
#     -lm -lc"
# )
# set(CMAKE_C_FLAGS "${STM32_COMPILER_OPTIONS} ${COMPILER_OPTIMISATION} -std=c11" CACHE INTERNAL "c compiler flags")
# 
# set(EXTRA_LINKER_FLAGS "-lm -lc --specs=nano.specs --specs=nosys.specs ")
# set(CMAKE_EXE_LINKER_FLAGS "${STM32_COMPILER_OPTIONS} ${LIBOPENCM3_LINKER_FLAGS} -Wl,-Map=linker.map -Wl,-cref " CACHE INTERNAL "exe link flags")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${EXTRA_LINKER_FLAGS} -T${LINKER_FILE}")

add_custom_target(flash
    DEPENDS ${PROJECT_NAME}.elf
    COMMAND avrdude 
)

# Add the source subdirectory
add_subdirectory("${CMAKE_SOURCE_DIR}/src")
