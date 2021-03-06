cmake_minimum_required(VERSION 3.0.0)

project(z-tools-filetransfer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_compile_options("-static")

find_package(Qt5Core)
find_package(Qt5Widgets)
find_package(Qt5Network)

# Archive build control
set(BUILD_TYPE "${BUILD_TYPE}")
if(BUILD_TYPE STREQUAL "TGZ")
    include(cmake/BuildSourceTgz.cmake)
else()
    if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
        include(cmake/BundleGenerater.cmake)
    elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
        include(cmake/DebGenerater.cmake)
    endif()
endif()


add_subdirectory(Components)

include_directories(
    src
    Components/FileTransferManager
)
set(QRC_SOURCES
    "resources/images.qrc"
)

add_executable(${PROJECT_NAME}
    "main.cpp"
    "src/mainwindow.h"
    "src/mainwindow.cpp"
    ${QRC_SOURCES}
)

target_link_libraries(${PROJECT_NAME}
    Qt5::Core Qt5::Widgets Qt5::Network

    FileTransferManager
)
