cmake_minimum_required(VERSION 3.19)
project(Application LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets UiTools Sql Network)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Authentication/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Service/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Delegate/*.cpp
)

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Authentication/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Service/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Delegate/*.h
    
    ${COMMON_INCLUDE_DIR}/Core/Data/Entity/*.h
    ${COMMON_INCLUDE_DIR}/Core/Data/Model/*.h
    ${COMMON_INCLUDE_DIR}/Worklist/Data/*.h
    ${COMMON_INCLUDE_DIR}/Worklist/Repository/*.h
    ${COMMON_INCLUDE_DIR}/Worklist/Specification/*.h
    ${COMMON_INCLUDE_DIR}/Core/Globalization/*.h
    ${CMAKE_VIEW_DIRECTORY}/Page/*.h
    ${CMAKE_VIEW_DIRECTORY}/Widget/*.h
    ${CMAKE_VIEW_DIRECTORY}/Dialog/*.h
    ${CMAKE_WORKLIST_DIRECTORY}/Connectivity/*.h
    ${CMAKE_WORKLIST_DIRECTORY}/Delegate/*.h
    ${CMAKE_WORKLIST_DIRECTORY}/Repository/*.h
    ${CMAKE_DEVICE_DIRECTORY}/Delegate/*.h
    ${CMAKE_DICOM_DIRECTORY}/Delegate/*.h
    ${CMAKE_PACS_DIRECTORY}/Delegate/*.h
    ${CMAKE_SCANPROTOCOL_DIRECTORY}/Delegate/*.h

)

qt_add_resources(MY_APP_RES "resources.qrc")

add_library(Application SHARED
    ${SOURCES}
    ${HEADERS}
    ${MY_APP_RES}
)

# Ensure that both DLL and LIB are built
set_target_properties(Application PROPERTIES
    WINDOWS_EXPORT_ALL_SYMBOLS TRUE  # Ensure that the import library is generated
)

# some of these libs might go to launch.
target_link_libraries(Application
    PRIVATE Qt6::Core Qt6::Widgets Qt6::UiTools Qt6::Sql Qt6::Network
    PUBLIC 
    Core 
    Common 
    View 
    Worklist 
    Device 
    Dicom 
    Pacs 
    ScanProtocol
)

target_include_directories(Application
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Authentication
        ${CMAKE_CURRENT_SOURCE_DIR}/Service 
        ${CMAKE_CURRENT_SOURCE_DIR}/Delegate 
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_VIEW_DIRECTORY}/Page
        ${CMAKE_VIEW_DIRECTORY}/Widget
        ${CMAKE_VIEW_DIRECTORY}/Dialog
        ${CMAKE_WORKLIST_DIRECTORY}/Connectivity
        ${CMAKE_WORKLIST_DIRECTORY}/Delegate
        ${CMAKE_WORKLIST_DIRECTORY}/Repository
        ${CMAKE_DEVICE_DIRECTORY}/Delegate
        ${CMAKE_DICOM_DIRECTORY}/Delegate
        ${CMAKE_PACS_DIRECTORY}/Delegate
        ${CMAKE_SCANPROTOCOL_DIRECTORY}/Delegate
)


# Ensure Application DLL and LIB go to the correct folders as set by the root CMake
# RUNTIME_OUTPUT_DIRECTORY is used to put output dlls in bin folder
set_target_properties(Application PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}  # .lib files
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}  # .dll files
#    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}  # .exe files (if any)
)
