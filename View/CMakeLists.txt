cmake_minimum_required(VERSION 3.19)
project(View LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets UiTools Sql Network)

set(CMAKE_AUTOUIC_SEARCH_PATHS    
    ${CMAKE_CURRENT_SOURCE_DIR}/Page    
    ${CMAKE_CURRENT_SOURCE_DIR}/Widget
    ${CMAKE_CURRENT_SOURCE_DIR}/Dialog    
)

qt_add_resources(MY_APP_RES "resources.qrc")

# all widgets are inserted in single place, 
# instead of dividing them into multiple subdirectories
# this is because we want to keep the structure flat for easier navigation
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Page/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Widget/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Dialog/*.cpp   
    ${CMAKE_CURRENT_SOURCE_DIR}/Delegate/*.cpp   
)

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Page/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Widget/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Dialog/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Delegate/*.h

    ${COMMON_INCLUDE_DIR}/Core/Data/Entity/*.h
    ${COMMON_INCLUDE_DIR}/Core/Data/Model/*.h
    ${COMMON_INCLUDE_DIR}/Core/Globalization/*.h
    ${COMMON_INCLUDE_DIR}/Worklist/Data/*.h
    ${COMMON_INCLUDE_DIR}/Worklist/Repository/*.h
    ${COMMON_INCLUDE_DIR}/Worklist/Specification/*.h
    ${COMMON_INCLUDE_DIR}/Device/*.h
    ${COMMON_INCLUDE_DIR}/Device/Data/Entity/*.h
    ${COMMON_INCLUDE_DIR}/Dicom/*.h
    ${COMMON_INCLUDE_DIR}/Dicom/Data/Entity/*.h
    ${COMMON_INCLUDE_DIR}/Pacs/*.h
    ${COMMON_INCLUDE_DIR}/Pacs/Data/Entity/*.h
   
)

file(GLOB_RECURSE UIS CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Page/*.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/Widget/*.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/Dialog/*.ui
)

qt_add_library(View SHARED
    ${SOURCES}
    ${HEADERS}
    ${MY_APP_RES}
    ${UIS}
)

# Ensure that both DLL and LIB are built
set_target_properties(View PROPERTIES
    WINDOWS_EXPORT_ALL_SYMBOLS TRUE  # Ensure that the import library is generated
)

# some of these libs might go to launch.
target_link_libraries(View
    PRIVATE 
    Qt6::Core Qt6::Widgets Qt6::UiTools Qt6::Network
    Core
    Common
)

target_include_directories(View
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Page
        ${CMAKE_CURRENT_SOURCE_DIR}/Widget
        ${CMAKE_CURRENT_SOURCE_DIR}/Dialog
        ${CMAKE_CURRENT_SOURCE_DIR}/Delegate
        ${COMMON_INCLUDE_DIR}/Core/Data/Entity
        ${COMMON_INCLUDE_DIR}/Core/Data/Model
        ${COMMON_INCLUDE_DIR}/Core/Globalization
        ${COMMON_INCLUDE_DIR}/Worklist/Data
        ${COMMON_INCLUDE_DIR}/Worklist/Data/Entity
        ${COMMON_INCLUDE_DIR}/Worklist/Repository
        ${COMMON_INCLUDE_DIR}/Worklist/Specification
        ${COMMON_INCLUDE_DIR}/Device
        ${COMMON_INCLUDE_DIR}/Device/Data/Entity
        ${COMMON_INCLUDE_DIR}/Dicom
        ${COMMON_INCLUDE_DIR}/Dicom/Data/Entity
        ${COMMON_INCLUDE_DIR}/Pacs
        ${COMMON_INCLUDE_DIR}/Pacs/Data/Entity
        ${COMMON_INCLUDE_DIR}/ScanProtocol
        ${COMMON_INCLUDE_DIR}/ScanProtocol/Data/Entity
        ${CMAKE_CURRENT_BINARY_DIR}
)

# Ensure View DLL and LIB go to the correct folders as set by the root CMake
# RUNTIME_OUTPUT_DIRECTORY is used to put output dlls in bin folder
set_target_properties(View PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}  # .lib files
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}  # .dll files
#    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}  # .exe files (if any)
)