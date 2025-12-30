cmake_minimum_required(VERSION 3.19)
project(ImageViewer LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets UiTools Sql Network OpenGL OpenGLWidgets)

set(CMAKE_AUTOUIC_SEARCH_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}/Data
    ${CMAKE_CURRENT_SOURCE_DIR}/Delegate
    ${CMAKE_CURRENT_SOURCE_DIR}/Repository
    ${CMAKE_CURRENT_SOURCE_DIR}/Service
    ${CMAKE_CURRENT_SOURCE_DIR}/Rendering
    ${CMAKE_CURRENT_SOURCE_DIR}/Tool
)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Data/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Delegate/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Repository/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Service/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Rendering/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Tool/*.cpp
)

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Data/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Delegate/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Repository/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Service/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Rendering/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Tool/*.h
)

add_library(ImageViewer SHARED
    ${SOURCES}
    ${HEADERS}
)

target_compile_definitions(ImageViewer PRIVATE IMAGEVIEWER_LIBRARY)

set_target_properties(ImageViewer PROPERTIES
    WINDOWS_EXPORT_ALL_SYMBOLS TRUE
)

target_link_libraries(ImageViewer
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::UiTools
        Qt6::Sql
        Qt6::Network
        Qt6::OpenGL
        Qt6::OpenGLWidgets
    PUBLIC
        Core
        Common
        View
        Dicom
        ${VTK_LIBRARIES}
        ${DCMTK_LIBRARIES}
        ${WIN_SDK_LIBRARIES}
)

target_include_directories(ImageViewer
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Data
        ${CMAKE_CURRENT_SOURCE_DIR}/Data/Entity
        ${CMAKE_CURRENT_SOURCE_DIR}/Delegate
        ${CMAKE_CURRENT_SOURCE_DIR}/Repository
        ${CMAKE_CURRENT_SOURCE_DIR}/Service
        ${CMAKE_CURRENT_SOURCE_DIR}/Rendering
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Tool
        ${CMAKE_CORE_DIRECTORY}/Data/Model
        ${CMAKE_CORE_DIRECTORY}/Log
        ${CMAKE_CORE_DIRECTORY}/Globalization
        ${VTK_INCLUDE_DIR}
        ${DCMTK_INCLUDE_DIR}
        ${OPENSSL_INCLUDE_DIR}
        ${COMMON_INCLUDE_DIR}/ImageViewer
        ${COMMON_INCLUDE_DIR}/Globalization
        ${COMMON_INCLUDE_DIR}/Specification
        ${COMMON_INCLUDE_DIR}/Worklist/Data/Entity
        ${COMMON_INCLUDE_DIR}/Worklist/Repository
        ${COMMON_INCLUDE_DIR}/Dicom/Data/Entity
)

# Output directories
set_target_properties(ImageViewer PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

install(TARGETS ImageViewer
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
