cmake_minimum_required(VERSION 3.19)
project(Etrek LANGUAGES CXX)

# Configure all output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)  # .lib files here
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)  # .dll files here
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})      # .exe here


# Globally declare the Core project location
set(CMAKE_CORE_DIRECTORY "${CMAKE_SOURCE_DIR}/Core")
set(CMAKE_VIEW_DIRECTORY "${CMAKE_SOURCE_DIR}/View")
set(CMAKE_DEVICE_DIRECTORY "${CMAKE_SOURCE_DIR}/Device")
set(CMAKE_WORKLIST_DIRECTORY "${CMAKE_SOURCE_DIR}/Worklist")
set(CMAKE_DICOM_DIRECTORY "${CMAKE_SOURCE_DIR}/Dicom")
set(CMAKE_PACS_DIRECTORY "${CMAKE_SOURCE_DIR}/Pacs")
set(CMAKE_SCANPROTOCOL_DIRECTORY "${CMAKE_SOURCE_DIR}/ScanProtocol")


# Define global (common) variables for all sub-projects
if(WIN32)

    set(WIN_DRIVER_DIR "${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/Windows")
    set(DCMTK_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/dcmtk/include")
    set(DCMTK_LIB_DIR "${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/dcmtk/lib")
    set(SPDLOG_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/spdlog/include")
    set(OPENSSL_LIB_DIR "${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/openssl/lib")
    set(OPENSSL_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/openssl/include")
    set(WIN_SDK_DIR "${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/windows/sdk") 

endif()


 set(COMMON_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Common/Include")

# copy assets to each subdirectory that needs shared assets
#if(NOT EXISTS "${CMAKE_SOURCE_DIR}/Core/Asset/Icon")
#    file(COPY "${CMAKE_SOURCE_DIR}/Common/Asset/Icon"
#         DESTINATION "${CMAKE_SOURCE_DIR}/Core/Asset"
#    )
#    message(STATUS "✅ Copied Icon directory to Core/Asset")
#else()
#    message(STATUS "⚠️ Icon directory already exists in Core/Asset")
#endif()

# Ensure the log directory exists in the build root
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/log")

# or use the following to Create development symlinks/copies
#configure_file(${CMAKE_SOURCE_DIR}/Common/Asset/Icon ${CMAKE_BINARY_DIR}/assets/Icon COPYONLY)


add_subdirectory(Common)
add_subdirectory(Core)
add_subdirectory(View)
add_subdirectory(Application)
add_subdirectory(Device)
add_subdirectory(Dicom)
add_subdirectory(Pacs)
add_subdirectory(ScanProtocol)
#add_subdirectory(ImageViewer)
add_subdirectory(Worklist)
add_subdirectory(Executable)

