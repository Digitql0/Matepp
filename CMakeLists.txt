cmake_minimum_required(VERSION 3.16)
project(MYAPP LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)

add_subdirectory(external/OTHERLIB)
add_subdirectory(external/OTHERLIB)

# Collect your own source files
file(GLOB_RECURSE SRC_FILES
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/*.c"
)

# Now add your executable target
add_executable(MYAPP ${SRC_FILES})
include_directories(${PROJECT_SOURCE_DIR}/include)
target_link_libraries(MYAPP PRIVATE OTHERLIB)
target_link_libraries(MYAPP PRIVATE OTHERLIB)
