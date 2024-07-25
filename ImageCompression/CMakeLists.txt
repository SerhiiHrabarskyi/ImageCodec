cmake_minimum_required(VERSION 3.16)

project(ImageCompression VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.2 REQUIRED COMPONENTS Quick)
find_package(Qt6 REQUIRED COMPONENTS QuickControls2)

set(EXTERNAL_LIBRARY_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/../ImageCodecLib")
set(EXTERNAL_LIBRARY_LIB "${PROJECT_SOURCE_DIR}/../ImageCodecLib/build/Desktop_Qt_6_7_2_MSVC2019_64bit-Debug/ImgCodec.lib")

include_directories(${EXTERNAL_LIBRARY_INCLUDE_DIR})
add_library(ImgCodec STATIC IMPORTED)
set_target_properties(ImgCodec PROPERTIES IMPORTED_LOCATION ${EXTERNAL_LIBRARY_LIB})

qt_add_executable(appImageCompression
    main.cpp
)

qt_add_qml_module(appImageCompression
    URI ImageCompression
    VERSION 1.0
    QML_FILES
        Main.qml
        SOURCES imagecodec.h imagecodec.cpp
        SOURCES imageprovider.h
        SOURCES imagereader.h imagereader.cpp
        QML_FILES FileSelectorRow.qml
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appImageCompression PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appImageCompression
    PRIVATE Qt6::Quick
    PRIVATE Qt6::QuickControls2
    PRIVATE ImgCodec
)

include(GNUInstallDirs)
install(TARGETS appImageCompression
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
