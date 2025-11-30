cmake_minimum_required(VERSION 3.19)
project(Etrek LANGUAGES CXX)

# Configure all output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)  # .lib files here
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)  # .dll files here
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})      # .exe here


# Globally declare the Core project location
set(CMAKE_Application_DIRECTORY "${CMAKE_SOURCE_DIR}/Application")
set(CMAKE_CORE_DIRECTORY "${CMAKE_SOURCE_DIR}/Core")
set(CMAKE_VIEW_DIRECTORY "${CMAKE_SOURCE_DIR}/View")
set(CMAKE_DEVICE_DIRECTORY "${CMAKE_SOURCE_DIR}/Device")
set(CMAKE_WORKLIST_DIRECTORY "${CMAKE_SOURCE_DIR}/Worklist")
set(CMAKE_DICOM_DIRECTORY "${CMAKE_SOURCE_DIR}/Dicom")
set(CMAKE_PACS_DIRECTORY "${CMAKE_SOURCE_DIR}/Pacs")
set(CMAKE_SCANPROTOCOL_DIRECTORY "${CMAKE_SOURCE_DIR}/ScanProtocol")


# ============================================================================
# Common Include Directory
# ============================================================================
set(COMMON_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Common/Include")

# ============================================================================
# Third-Party Library Configuration
# ============================================================================
# find_package calls must be at root level to make targets available to all subdirectories
# Library paths and configurations are organized in ThirdPartyLibraries/CMakeLists.txt

# --- Qt Configuration (root level for target visibility) ---
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets UiTools Sql Network OpenGL OpenGLWidgets)
message(STATUS "✓ Qt ${Qt6_VERSION} configured")

# --- Third-Party Library Paths ---
# DCMTK, OpenSSL, Windows SDK paths are configured in ThirdPartyLibraries/
add_subdirectory(ThirdPartyLibraries)

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

