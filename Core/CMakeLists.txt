cmake_minimum_required(VERSION 3.19)
project(Core LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets UiTools Sql Network)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Data/Entity/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Data/Model/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Dialog/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Globalization/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Log/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Repository/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Security/Authentication/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Security/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Setting/*.cpp
)

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Data/Entity/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Data/Model/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Dialog/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Log/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Repository/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Security/Authentication/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Security/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Setting/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Specification/*.h
    #${COMMON_INCLUDE_DIR}/Data/Core/*.h
    #${COMMON_INCLUDE_DIR}/Data/Worklist/*.h
    #${COMMON_INCLUDE_DIR}/Globalization/*.h
    ${COMMON_INCLUDE_DIR}/Specification/*.h
    ${COMMON_INCLUDE_DIR}/Core/Data/Entity/*.h
    ${COMMON_INCLUDE_DIR}/Core/Data/Model/*.h
    ${COMMON_INCLUDE_DIR}/Core/Globalization/*.h
    ${COMMON_INCLUDE_DIR}/Worklist/Data/*.h
    ${COMMON_INCLUDE_DIR}/Worklist/Repository/*.h
    ${COMMON_INCLUDE_DIR}/Worklist/Specification/*.h
)

qt_add_resources(MY_APP_RES "resources.qrc")

qt_add_library(Core SHARED
    ${SOURCES}
    ${HEADERS}
    ${MY_APP_RES}
)

# Define an interface library for SPDLOG (no actual implementation, just the include path)
add_library(SPDLOG INTERFACE)
target_include_directories(SPDLOG INTERFACE ${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/spdlog/include)

# You can do the same for other dependencies (e.g., DCMTK, OpenSSL)
#add_library(DCMTK INTERFACE)
#target_include_directories(DCMTK INTERFACE ${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/dcmtk/include)

# You can continue adding more such interface libraries...

# Ensure that both DLL and LIB are built
set_target_properties(Core PROPERTIES
    WINDOWS_EXPORT_ALL_SYMBOLS TRUE  # Ensure that the import library is generated
)

# some of these libs might go to launch.
target_link_libraries(Core
    PRIVATE 
    #Qt::Core Qt::Widgets Qt::UiTools Qt::Sql Qt::Network
    Qt6::Core Qt6::Sql Qt6::Network    
    ${OPENSSL_LIB_DIR}/libcrypto.lib
    ${OPENSSL_LIB_DIR}/libssl.lib
    ${DCMTK_LIB_DIR}/dcmnet.lib
    ${DCMTK_LIB_DIR}/dcmdata.lib
    ${DCMTK_LIB_DIR}/oflog.lib
    ${DCMTK_LIB_DIR}/ofstd.lib
    ${DCMTK_LIB_DIR}/dcmimgle.lib
    ${DCMTK_LIB_DIR}/dcmsr.lib
    ${DCMTK_LIB_DIR}/dcmnet.lib
    ${DCMTK_LIB_DIR}/dcmdata.lib
    ${DCMTK_LIB_DIR}/oflog.lib
    ${DCMTK_LIB_DIR}/ofstd.lib
    ${DCMTK_LIB_DIR}/dcmimgle.lib
    ${DCMTK_LIB_DIR}/dcmsr.lib
    ${WIN_SDK_DIR}/Iphlpapi.lib
    ${WIN_SDK_DIR}/Netapi32.lib
    PUBLIC
    SPDLOG
    Common
)


target_include_directories(Core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Data/Entity
        ${CMAKE_CURRENT_SOURCE_DIR}/Data/Model
        ${CMAKE_CURRENT_SOURCE_DIR}/Dialog
        ${CMAKE_CURRENT_SOURCE_DIR}/Globalization
        ${CMAKE_CURRENT_SOURCE_DIR}/Log
        ${CMAKE_CURRENT_SOURCE_DIR}/Repository
        ${CMAKE_CURRENT_SOURCE_DIR}/Security/Authentication
        ${CMAKE_CURRENT_SOURCE_DIR}/Security
        ${CMAKE_CURRENT_SOURCE_DIR}/Service
        ${CMAKE_CURRENT_SOURCE_DIR}/Setting
        ${CMAKE_CURRENT_SOURCE_DIR}/Specification
        ${DCMTK_INCLUDE_DIR}
        ${SPDLOG_INCLUDE_DIR}
        ${OPENSSL_INCLUDE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}  
        ${COMMON_INCLUDE_DIR}/Specification
        ${COMMON_INCLUDE_DIR}/Core/Data/Entity
        ${COMMON_INCLUDE_DIR}/Core/Data/Model
        ${COMMON_INCLUDE_DIR}/Core/Globalization
        ${COMMON_INCLUDE_DIR}/Worklist/Data
        ${COMMON_INCLUDE_DIR}/Worklist/Repository
        ${COMMON_INCLUDE_DIR}/Worklist/Specification
)


# Ensure Core DLL and LIB go to the correct folders as set by the root CMake
# RUNTIME_OUTPUT_DIRECTORY is used to put output dlls in bin folder
set_target_properties(Core PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}  # .lib files
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}  # .dll files
#    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}  # .exe files (if any)
)


# Set the source and destination directories
set(LANG_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Globalization/Lan")
set(LANG_OUTPUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lang")
set(SETTING_OUTPUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/setting")

# Get all language directories (e.g., en, fr, etc.)
file(GLOB LANG_DIRS RELATIVE "${LANG_SOURCE_DIR}" "${LANG_SOURCE_DIR}/*")

# Prepare a list to collect all copy commands
set(LANG_COPY_COMMANDS)

foreach(LANG ${LANG_DIRS})
    if(IS_DIRECTORY "${LANG_SOURCE_DIR}/${LANG}")
        # Create the output directory for this language
        file(MAKE_DIRECTORY "${LANG_OUTPUT_DIR}/${LANG}")

        # Copy all files from the language directory (e.g., messages.json)
        add_custom_command(
            TARGET Core POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${LANG_SOURCE_DIR}/${LANG}"
                "${LANG_OUTPUT_DIR}/${LANG}"
            COMMENT "Copying language files for ${LANG}"
        )
    endif()
endforeach()


# Copy all setting resource files after build
add_custom_command(
    TARGET Core POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Setting/Resources"
        "${SETTING_OUTPUT_DIR}"
    COMMENT "Copying setting resource files"
)


#install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/Script/setup_database.sql
#    DESTINATION ${CMAKE_INSTALL_BINDIR}/Script
#)
