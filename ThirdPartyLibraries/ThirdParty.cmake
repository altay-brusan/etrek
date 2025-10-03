# ThirdParty.cmake - Handles third-party DLLs for Windows (Qt 6.5.3)

# Ensure Qt version is 6.5.3
if (NOT Qt6Core_VERSION VERSION_EQUAL "6.5.3")
    message(FATAL_ERROR "This project requires Qt 6.5.3. Found: ${Qt6Core_VERSION}")
endif()


# Only apply this logic on Windows
if (WIN32)
    message(STATUS "Configuring third-party DLLs for Windows...")

    set(THIRD_PARTY_DIR "${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/Windows")

    set(SPDLOG_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/spdlog/include")

    set(OPENSSL_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/openssl/include")

    set(OPENSSL_LIB_DIR "${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/openssl/lib")

    set(DCMTK_LIB_DIR "${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/dcmtk/lib")

    set(DCMTK_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/dcmtk/include")

    set(WIN_SDK_DIR "${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/windows/sdk")


    # List of DLLs to copy to output directory
    set(THIRD_PARTY_DLLS
        "${THIRD_PARTY_DIR}/libcrypto-3-x64.dll"
        "${THIRD_PARTY_DIR}/libmysql.dll"
        "${THIRD_PARTY_DIR}/libssl-3-x64.dll"
    )

    # Add the list to be copied post-build
    foreach(dll ${THIRD_PARTY_DLLS})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${dll}"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        )
    endforeach()

    # Also copy the MySQL Qt plugin to sqldrivers/ inside build output
    set(QT_MYSQL_PLUGIN "${THIRD_PARTY_DIR}/sqldrivers/qsqlmysql.dll")
    set(QT_MYSQL_PLUGIN_DIR "$<TARGET_FILE_DIR:${PROJECT_NAME}>/sqldrivers")

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${QT_MYSQL_PLUGIN_DIR}"
    )

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${QT_MYSQL_PLUGIN}"
        "${QT_MYSQL_PLUGIN_DIR}"
    )

    message(STATUS "✅ Third-party DLLs configured for Windows build")
else()
    message(WARNING "⚠️ Third-party library setup skipped (Not a Windows platform)")
endif()
