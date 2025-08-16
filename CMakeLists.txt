cmake_minimum_required(VERSION 3.16)
project(matepp LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)

# Collect your own source files
file(GLOB_RECURSE SRC_FILES
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/*.c"
)

# Now add your executable target
add_executable(matepp ${SRC_FILES})
include_directories(${PROJECT_SOURCE_DIR}/include)
