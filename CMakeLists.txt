cmake_minimum_required(VERSION 3.19)

# set project
project(coco-air-sensor)

# configure languages
enable_language(CXX)
set(CMAKE_CXX_STANDARD 20)

# build type
message("*** Type: ${CMAKE_BUILD_TYPE}${CMAKE_CONFIGURATION_TYPES}")
add_compile_definitions($<$<CONFIG:Debug>:DEBUG>)

# compiler
message("*** Compiler: ${CMAKE_CXX_COMPILER_ID}")
message("*** Crosscompiling: ${CMAKE_CROSSCOMPILING}")

# enable address sanitizer
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")

# platform
#message("*** OS: ${OS}")
message("*** Platform: ${PLATFORM}")

# dependencies
find_package(coco CONFIG)
find_package(coco-loop CONFIG)
find_package(coco-device CONFIG)
find_package(coco-devboards CONFIG)
find_package(coco-spi CONFIG)
find_package(coco-i2c CONFIG)
find_package(coco-mono-display CONFIG)


add_subdirectory(coco)


add_subdirectory(test)
