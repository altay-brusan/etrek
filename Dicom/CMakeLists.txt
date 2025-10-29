cmake_minimum_required(VERSION 3.19)
project(Dicom LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets UiTools Sql Network)

set(CMAKE_AUTOUIC_SEARCH_PATHS
    #${CMAKE_CURRENT_SOURCE_DIR}/Data    
    ${CMAKE_CURRENT_SOURCE_DIR}/Delegate
    ${CMAKE_CURRENT_SOURCE_DIR}/Repository
)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    #${CMAKE_CURRENT_SOURCE_DIR}/Data/*.cpp    
    ${CMAKE_CURRENT_SOURCE_DIR}/Delegate/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Repository/*.cpp    
)

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
    #${CMAKE_CURRENT_SOURCE_DIR}/Data/*.h    
    ${CMAKE_CURRENT_SOURCE_DIR}/Delegate/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Repository/*.h    
    
    ${COMMON_INCLUDE_DIR}/*.h
    ${COMMON_INCLUDE_DIR}/Dicom/*.h
    ${COMMON_INCLUDE_DIR}/Dicom/Data/Entity/*.h
)

add_library(Dicom SHARED
    ${SOURCES}
    ${HEADERS}
)

 # Ensure that both DLL and LIB are built
set_target_properties(Dicom PROPERTIES
    WINDOWS_EXPORT_ALL_SYMBOLS TRUE  
)

target_link_libraries(Dicom
    PRIVATE Qt6::Core Qt6::Widgets Qt6::UiTools Qt6::Sql Qt6::Network
    PUBLIC
    Core
    Common
    View
)


target_include_directories(Dicom
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Data/Entity
    ${CMAKE_CURRENT_SOURCE_DIR}/Data
    ${CMAKE_CURRENT_SOURCE_DIR}/Delegate
    ${CMAKE_CURRENT_SOURCE_DIR}/Repository
    ${COMMON_INCLUDE_DIR}/Dicom/
    ${COMMON_INCLUDE_DIR}/Dicom/Data/Entity
    ${COMMON_INCLUDE_DIR}/Core/Data/Entity
)

install(TARGETS Dicom
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
