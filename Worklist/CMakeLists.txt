cmake_minimum_required(VERSION 3.19)
project(Worklist LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets UiTools Sql Network)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Connectivity/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Delegate/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Repository/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Service/*.cpp
    ${COMMON_INCLUDE_DIR}/Dicom/Repository/*.cpp
)

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Connectivity/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Delegate/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Repository/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Service/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Mapping/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Utility/*.h

    ${COMMON_INCLUDE_DIR}/*.h

    ${COMMON_INCLUDE_DIR}/Core/Data/Entity/*.h
    ${COMMON_INCLUDE_DIR}/Core/Data/Model/*.h
    ${COMMON_INCLUDE_DIR}/Core/Globalization/*.h

    ${COMMON_INCLUDE_DIR}/Globalization/*.h
    ${COMMON_INCLUDE_DIR}/Specification/*.h
    ${COMMON_INCLUDE_DIR}/Worklist/Data/Entity/*.h
    ${COMMON_INCLUDE_DIR}/Worklist/Repository/*.h
    
    ${COMMON_INCLUDE_DIR}/Dicom/Repository/*.h


    ${CMAKE_VIEW_DIRECTORY}/Page/*.h
    ${CMAKE_VIEW_DIRECTORY}/Widget/*.h
    ${CMAKE_VIEW_DIRECTORY}/Dialog/*.h

    ${CMAKE_CORE_DIRECTORY}/Data/Model/*.h
)

add_library(Worklist SHARED
    ${SOURCES}
    ${HEADERS}
)


# Ensure that both DLL and LIB are built
set_target_properties(Worklist PROPERTIES
    WINDOWS_EXPORT_ALL_SYMBOLS TRUE  
)


target_link_libraries(Worklist
    PRIVATE Qt6::Core Qt6::Widgets Qt6::UiTools Qt6::Sql Qt6::Network
    ${DCMTK_LIB_DIR}/dcmnet.lib
    ${DCMTK_LIB_DIR}/dcmdata.lib
    ${DCMTK_LIB_DIR}/oflog.lib
    ${DCMTK_LIB_DIR}/ofstd.lib
    ${DCMTK_LIB_DIR}/dcmimgle.lib
    ${DCMTK_LIB_DIR}/dcmsr.lib
    ${WIN_SDK_DIR}/Iphlpapi.lib
    ${WIN_SDK_DIR}/Netapi32.lib
    PUBLIC
    Core
    Common
    View
    ScanProtocol
    Dicom
)

target_include_directories(Worklist
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Delegate
    ${CMAKE_CURRENT_SOURCE_DIR}/Repository
    ${CMAKE_CURRENT_SOURCE_DIR}/Service
    ${CMAKE_CURRENT_SOURCE_DIR}/Mapping
    ${CMAKE_CURRENT_SOURCE_DIR}/Utility
    ${CMAKE_CURRENT_BINARY_DIR}
    PUBLIC
    ${CMAKE_CORE_DIRECTORY}/Data/Model
    ${CMAKE_CORE_DIRECTORY}/Log
    ${CMAKE_CORE_DIRECTORY}/Globalization
    
    ${DCMTK_INCLUDE_DIR}
    ${COMMON_INCLUDE_DIR}/Globalization
    ${COMMON_INCLUDE_DIR}/Specification
    ${COMMON_INCLUDE_DIR}/Worklist/Data/Entity
    ${COMMON_INCLUDE_DIR}/Worklist/Repository
    ${COMMON_INCLUDE_DIR}/Dicom/Repository

    ${COMMON_INCLUDE_DIR}/ScanProtocol
    ${COMMON_INCLUDE_DIR}/ScanProtocol/Data/Entity


    ${CMAKE_VIEW_DIRECTORY}/Page
    ${CMAKE_VIEW_DIRECTORY}/Widget
    ${CMAKE_VIEW_DIRECTORY}/Dialog

    ${DCMTK_INCLUDE_DIR}
)

# Ensure Worklist DLL and LIB go to the correct folders as set by the root CMake
# RUNTIME_OUTPUT_DIRECTORY is used to put output dlls in bin folder
set_target_properties(Worklist PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}  # .lib files
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}  # .dll files
#    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}  # .exe files (if any)
)

# should be copied to a final installation directory when the cmake --install command is run
#install(TARGETS Worklist
#    BUNDLE DESTINATION .
#    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#)
