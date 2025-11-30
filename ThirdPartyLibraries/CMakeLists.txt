# ============================================================================
# Third-Party Library Configuration for Windows
# ============================================================================
# This file configures paths for third-party libraries and DLL copying
# Note: find_package(Qt6) is called at root CMakeLists.txt for target visibility
# Note: find_package(VTK) is called in View/CMakeLists.txt (module-specific)

if(WIN32)
    message(STATUS "Configuring Windows third-party libraries...")

    # ========================================================================
    # Library Path Configuration
    # ========================================================================
    
    # Header-only libraries
    set(SPDLOG_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/spdlog/include")

    # DCMTK
    set(DCMTK_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/dcmtk/include")
    set(DCMTK_LIB_DIR "${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/dcmtk/lib")
    set(DCMTK_LIBRARIES
        ${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/dcmtk/lib/dcmnet.lib
        ${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/dcmtk/lib/dcmdata.lib
        ${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/dcmtk/lib/oflog.lib
        ${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/dcmtk/lib/ofstd.lib
        ${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/dcmtk/lib/dcmimgle.lib
        ${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/dcmtk/lib/dcmsr.lib
    )

    # OpenSSL
    set(OPENSSL_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/openssl/include")
    set(OPENSSL_LIB_DIR "${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/openssl/lib")
    set(OPENSSL_LIBRARIES
        ${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/openssl/lib/libcrypto.lib
        ${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/openssl/lib/libssl.lib
    )

    # Windows SDK
    set(WIN_SDK_DIR "${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/windows/sdk")
    set(WIN_SDK_LIBRARIES
        ${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/windows/sdk/Iphlpapi.lib
        ${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/windows/sdk/Netapi32.lib
    )

    # VTK
    set(VTK_DIR "${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/vtk/lib/cmake/vtk-9.5")
    set(VTK_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/vtk/include")
    set(VTK_LIB_DIR "${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/vtk/lib")
    set(VTK_BIN_DIR "${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/vtk/bin")

    # Windows Driver
    set(WIN_DRIVER_DIR "${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/Windows")

    message(STATUS "✓ DCMTK configured")
    message(STATUS "✓ OpenSSL configured")
    message(STATUS "✓ spdlog configured (header-only)")
    message(STATUS "✓ Windows SDK configured")
    message(STATUS "✓ VTK configured")

    # ========================================================================
    # DLL Copying Configuration (Qt 6.5.3 Required)
    # ========================================================================
    
    # Ensure Qt version is 6.5.3
    if(NOT Qt6Core_VERSION VERSION_EQUAL "6.5.3")
        message(FATAL_ERROR "This project requires Qt 6.5.3. Found: ${Qt6Core_VERSION}")
    endif()

    # Function to configure DLL copying for a target
    function(configure_third_party_dlls TARGET_NAME)
        message(STATUS "Configuring third-party DLLs for ${TARGET_NAME}...")

        # List of DLLs to copy
        set(THIRD_PARTY_DLLS
            "${WIN_DRIVER_DIR}/libcrypto-3-x64.dll"
            "${WIN_DRIVER_DIR}/libmysql.dll"
            "${WIN_DRIVER_DIR}/libssl-3-x64.dll"
        )

        # Add all VTK DLLs
        file(GLOB VTK_DLLS "${VTK_BIN_DIR}/*.dll")
        list(APPEND THIRD_PARTY_DLLS ${VTK_DLLS})

        # Copy each DLL to output directory
        foreach(dll ${THIRD_PARTY_DLLS})
            add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${dll}"
                "$<TARGET_FILE_DIR:${TARGET_NAME}>"
            )
        endforeach()

        # Copy MySQL Qt plugin to sqldrivers/
        set(QT_MYSQL_PLUGIN "${WIN_DRIVER_DIR}/sqldrivers/qsqlmysql.dll")
        set(QT_MYSQL_PLUGIN_DIR "$<TARGET_FILE_DIR:${TARGET_NAME}>/sqldrivers")

        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${QT_MYSQL_PLUGIN_DIR}"
        )

        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${QT_MYSQL_PLUGIN}"
            "${QT_MYSQL_PLUGIN_DIR}"
        )

        message(STATUS "✓ Third-party DLLs configured for ${TARGET_NAME}")
    endfunction()

else()
    message(WARNING "⚠️ Third-party library setup skipped (Not a Windows platform)")
endif()
