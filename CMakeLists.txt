cmake_minimum_required(VERSION 2.8.4)
set(TARGET_NAME final)
SET(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})
project(${TARGET_NAME})

# Set build flags
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

# Set directories
set(SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
set(LIBRARY_DIR ${PROJECT_SOURCE_DIR}/lib)

# Find source files
file(GLOB SOURCE_FILES ${SOURCE_DIR}/*.cpp)

# Create build rules
include_directories(${SOURCE_DIR} ${LIBRARY_DIR})
add_executable(${TARGET_NAME} ${SOURCE_FILES})

if(APPLE)
    find_library(COCOA Cocoa)
    find_library(IOKIT IOKit)
    find_library(CORE_VIDEO CoreVideo)
    target_link_libraries(${TARGET_NAME} ${COCOA} ${IOKIT} ${CORE_VIDEO})
endif()

# Find libraries
find_library(OPENGL OpenGL)
find_library(GLFW glfw3 PATHS ${LIBRARY_DIR})
target_link_libraries(${TARGET_NAME} ${OPENGL} ${GLFW})

install(TARGETS ${TARGET_NAME} RUNTIME DESTINATION deploy)
