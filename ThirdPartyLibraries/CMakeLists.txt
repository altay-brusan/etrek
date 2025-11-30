cmake_minimum_required(VERSION 3.19)

# ============================================================================
# Third-Party Library Path Configuration
# ============================================================================
# This file sets up library paths and creates library list variables
# Note: find_package(Qt6) is called at root CMakeLists.txt for target visibility
# Note: find_package(VTK) is called in View/CMakeLists.txt (module-specific)

# --- Windows-specific Third-Party Libraries ---
if(WIN32)
    message(STATUS "Configuring Windows third-party libraries...")

    # Header-only libraries
    set(SPDLOG_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/spdlog/include" PARENT_SCOPE)

    # DCMTK
    set(DCMTK_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/dcmtk/include" PARENT_SCOPE)
    set(DCMTK_LIB_DIR "${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/dcmtk/lib" PARENT_SCOPE)
    set(DCMTK_LIBRARIES
        ${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/dcmtk/lib/dcmnet.lib
        ${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/dcmtk/lib/dcmdata.lib
        ${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/dcmtk/lib/oflog.lib
        ${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/dcmtk/lib/ofstd.lib
        ${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/dcmtk/lib/dcmimgle.lib
        ${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/dcmtk/lib/dcmsr.lib
        PARENT_SCOPE
    )

    # OpenSSL
    set(OPENSSL_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/openssl/include" PARENT_SCOPE)
    set(OPENSSL_LIB_DIR "${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/openssl/lib" PARENT_SCOPE)
    set(OPENSSL_LIBRARIES
        ${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/openssl/lib/libcrypto.lib
        ${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/openssl/lib/libssl.lib
        PARENT_SCOPE
    )

    # Windows SDK
    set(WIN_SDK_DIR "${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/windows/sdk" PARENT_SCOPE)
    set(WIN_SDK_LIBRARIES
        ${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/windows/sdk/Iphlpapi.lib
        ${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/windows/sdk/Netapi32.lib
        PARENT_SCOPE
    )

    # Windows Driver
    set(WIN_DRIVER_DIR "${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/Windows" PARENT_SCOPE)

    message(STATUS "✓ DCMTK configured")
    message(STATUS "✓ OpenSSL configured")
    message(STATUS "✓ spdlog configured (header-only)")
    message(STATUS "✓ Windows SDK configured")
endif()

message(STATUS "✓ All third-party libraries configured successfully")
