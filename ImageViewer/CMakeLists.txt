cmake_minimum_required(VERSION 3.19)
project(ImageViewer LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets UiTools Sql Network)

set(CMAKE_AUTOUIC_SEARCH_PATHS
    #${CMAKE_CURRENT_SOURCE_DIR}/Data    
    ${CMAKE_CURRENT_SOURCE_DIR}/Delegate
    ${CMAKE_CURRENT_SOURCE_DIR}/Repository
    #${CMAKE_CURRENT_SOURCE_DIR}/View
)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    #${CMAKE_CURRENT_SOURCE_DIR}/Data/*.cpp    
    ${CMAKE_CURRENT_SOURCE_DIR}/Delegate/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Repository/*.cpp
    #${CMAKE_CURRENT_SOURCE_DIR}/View/Widget/*.cpp
)

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
    #${CMAKE_CURRENT_SOURCE_DIR}/Data/*.h    
    ${CMAKE_CURRENT_SOURCE_DIR}/Delegate/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Repository/*.h
    #${CMAKE_CURRENT_SOURCE_DIR}/View/Widget/*.h  
)

#file(GLOB_RECURSE UIS CONFIGURE_DEPENDS
#    ${CMAKE_CURRENT_SOURCE_DIR}/View/Page/*.ui
#    ${CMAKE_CURRENT_SOURCE_DIR}/View/Widget/*.ui
#)

add_library(ImageViewer SHARED
    ${SOURCES}
    ${HEADERS}
)

# Ensure that both DLL and LIB are built
set_target_properties(ImageViewer PROPERTIES
    WINDOWS_EXPORT_ALL_SYMBOLS TRUE  
)

target_link_libraries(ImageViewer
    PRIVATE Qt6::Core Qt6::Widgets Qt6::UiTools Qt6::Sql Qt6::Network
    PUBLIC
    Core
    Common
    View
)



target_include_directories(ImageViewer
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Delegate
    ${CMAKE_CURRENT_SOURCE_DIR}/Repository
    
    PUBLIC
    ${CMAKE_CORE_DIRECTORY}/Data/Model
    ${CMAKE_CORE_DIRECTORY}/Log
    ${CMAKE_CORE_DIRECTORY}/Globalization
    
    ${DCMTK_INCLUDE_DIR}
    ${COMMON_INCLUDE_DIR}/Globalization
    ${COMMON_INCLUDE_DIR}/Specification
    ${COMMON_INCLUDE_DIR}/Worklist/Data/Entity
    ${COMMON_INCLUDE_DIR}/Worklist/Repository
)

install(TARGETS ImageViewer
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
