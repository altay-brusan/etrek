cmake_minimum_required(VERSION 3.19)
project(Etrek LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets UiTools Sql Network)

qt_standard_project_setup()

# Set output directories for the executable and libraries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})  # Etrek.exe will go to the root
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)  # All DLLs will go to 'bin/'


# Collect source files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)
# collect header files
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

qt_add_executable(Etrek
    main.cpp
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(Etrek
    PRIVATE Qt6::Core Qt6::Widgets Qt6::UiTools Qt6::Sql Qt6::Network 
    PRIVATE Application
    PRIVATE Common Core Application
)

target_include_directories(Etrek
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Service
        ${CMAKE_SOURCE_DIR}/Application/Service
        ${CMAKE_CURRENT_BINARY_DIR} 
)

# this section must be executed after executable is created
if(WIN32)
    # List of DLLs to copy to output directory
    set(WIN_DRIVER_DLL
        "${WIN_DRIVER_DIR}/libcrypto-3-x64.dll"
        "${WIN_DRIVER_DIR}/libmysql.dll"
        "${WIN_DRIVER_DIR}/libssl-3-x64.dll"
    )
    
    # Add the list to be copied post-build
    foreach(dll ${WIN_DRIVER_DLL})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${dll}"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        )
    endforeach()
endif()

# Post-build step to copy all DLLs to the 'bin' folder
#add_custom_command(TARGET Etrek POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy
#    $<TARGET_FILE:Core> ${CMAKE_BINARY_DIR}/bin  # Copy Core.dll to 'bin/'
#    # Add any additional DLLs here if needed, for example:
#    # COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:OtherLibrary> ${CMAKE_BINARY_DIR}/bin
#)
