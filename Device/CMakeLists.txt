cmake_minimum_required(VERSION 3.19)
project(Device LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets UiTools Sql Network)

set(CMAKE_AUTOUIC_SEARCH_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}/Delegate
    ${CMAKE_CURRENT_SOURCE_DIR}/Repository
)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Delegate/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Repository/*.cpp
)

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Delegate/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Repository/*.h

    ${COMMON_INCLUDE_DIR}/*.h
    ${COMMON_INCLUDE_DIR}/Device/*.h
    ${COMMON_INCLUDE_DIR}/Device/Data/Entity/*.h
)

 add_library(Device SHARED
     ${SOURCES}
     ${HEADERS}
 )

 # Ensure that both DLL and LIB are built
set_target_properties(Device PROPERTIES
    WINDOWS_EXPORT_ALL_SYMBOLS TRUE  
)


target_link_libraries(Device
    PRIVATE Qt6::Core Qt6::Widgets Qt6::UiTools Qt6::Sql Qt6::Network
    PUBLIC
    Core
    Common
    View
)

target_include_directories(Device
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Delegate
    ${CMAKE_CURRENT_SOURCE_DIR}/Repository
    
    ${CMAKE_CORE_DIRECTORY}/Data/Model
    ${CMAKE_CORE_DIRECTORY}/Log
    ${CMAKE_CORE_DIRECTORY}/Globalization
    
    ${DCMTK_INCLUDE_DIR}
    ${COMMON_INCLUDE_DIR}/Globalization
    ${COMMON_INCLUDE_DIR}/Specification
    ${COMMON_INCLUDE_DIR}/Device/
    ${COMMON_INCLUDE_DIR}/Device/Data/Entity
)

#install(TARGETS Device
#    BUNDLE DESTINATION .
#    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#)
