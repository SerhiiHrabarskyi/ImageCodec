cmake_minimum_required(VERSION 3.14)

project(ImageCompressionConsole LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui)

set(EXTERNAL_LIBRARY_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/../ImageCodecLib")
set(EXTERNAL_LIBRARY_LIB "${PROJECT_SOURCE_DIR}/../ImageCodecLib/build/Desktop_Qt_6_7_2_MSVC2019_64bit-Debug/ImgCodec.lib")

include_directories(${EXTERNAL_LIBRARY_INCLUDE_DIR})
add_library(ImgCodec STATIC IMPORTED)
set_target_properties(ImgCodec PROPERTIES IMPORTED_LOCATION ${EXTERNAL_LIBRARY_LIB})

add_executable(ImageCompressionConsole
  main.cpp
)
target_link_libraries(ImageCompressionConsole
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    ImgCodec
)

include(GNUInstallDirs)
install(TARGETS ImageCompressionConsole
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
