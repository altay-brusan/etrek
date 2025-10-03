cmake_minimum_required(VERSION 3.16)

project(SecurityModuleTest LANGUAGES CXX)

enable_testing()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Test Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test Sql network)



set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

get_filename_component(ETREK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../" ABSOLUTE)

set(SPDLOG_INCLUDE_DIR "${ETREK_DIR}/ThirdPartyLibraries/spdlog/include")
set(DCMTK_LIB_DIR "${ETREK_DIR}/ThirdPartyLibraries/dcmtk/lib")
set(DCMTK_INCLUDE_DIR "${ETREK_DIR}/ThirdPartyLibraries/dcmtk/include")

# Manually include only the files needed for test
set(SOURCES
    ${ETREK_DIR}/Security/SecurityServiceProvider.cpp
    ${ETREK_DIR}/Security/Crypto/AesEncryptionUtils.cpp
    ${ETREK_DIR}/Service/SettingProvider.cpp
    ${ETREK_DIR}/Service/LoggerProvider.cpp
    ${ETREK_DIR}/Service/AppLogger.cpp
    ${ETREK_DIR}/Service/AppLoggerFactory.cpp
    ${ETREK_DIR}/Service/DatabaseInitializer.cpp
    ${ETREK_DIR}/Service/TranslationProvider.cpp
    ${ETREK_DIR}/Service/WorklistQueryService.cpp
    ${ETREK_DIR}/Service/RisManager.cpp
    ${ETREK_DIR}/Model/DatabaseConnectionSetting.cpp
    ${ETREK_DIR}/Model/FileLoggerSetting.cpp
    ${ETREK_DIR}/Model/RisConnectionEntityModel.cpp
    ${ETREK_DIR}/Repository/AuthenticationRepository.cpp
    ${ETREK_DIR}/Repository/WorklistRepository.cpp
)


file(GLOB TEST_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/tst_*.cpp
)

# Define shared includes and libraries
set(TEST_INCLUDE_DIRS
    ${ETREK_DIR}
    ${ETREK_DIR}/Security
    ${ETREK_DIR}/Security/Auth
    ${ETREK_DIR}/Security/Crypto
    ${ETREK_DIR}/Service
    ${ETREK_DIR}/Model
    ${ETREK_DIR}/Model/Entity
    ${ETREK_DIR}/Repository
    ${ETREK_DIR}/Utility
    ${ETREK_DIR}/Specification
    ${OPENSSL_INCLUDE_DIR}
    ${SPDLOG_INCLUDE_DIR}
    ${DCMTK_INCLUDE_DIR}
)

set(TEST_LIBS
    Qt${QT_VERSION_MAJOR}::Test
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Network
    ${OPENSSL_LIB_DIR}/libcrypto.lib
    ${OPENSSL_LIB_DIR}/libssl.lib
    ${DCMTK_LIB_DIR}/dcmnet.lib
    ${DCMTK_LIB_DIR}/dcmdata.lib
    ${DCMTK_LIB_DIR}/oflog.lib
    ${DCMTK_LIB_DIR}/ofstd.lib
    ${DCMTK_LIB_DIR}/dcmimgle.lib
    ${DCMTK_LIB_DIR}/dcmsr.lib
    ${WIN_SDK_DIR}/Iphlpapi.lib
    ${WIN_SDK_DIR}/Netapi32.lib
)

# Copy setup_database.sql to the test binary directory
set(TEST_SQL_SOURCE "${ETREK_DIR}/Script/setup_database.sql")
set(TEST_SQL_DEST_NAME "temp_test_script.sql")  # name you use in QFile()


# Loop through each test file and create a separate executable
foreach(TEST_SRC ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SRC} NAME_WE) # e.g., tst_SecurityServiceProvider
    add_executable(${TEST_NAME} ${TEST_SRC} ${SOURCES}
        ${ETREK_DIR}/resources.qrc)
    target_include_directories(${TEST_NAME} PRIVATE ${TEST_INCLUDE_DIRS})
    target_link_libraries(${TEST_NAME} PRIVATE ${TEST_LIBS})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()

if (WIN32)
    # Paths to third-party DLLs and Qt MySQL plugin
    set(TEST_THIRD_PARTY_DIR "${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/Windows")
    set(TEST_THIRD_PARTY_WIN_SDK_DIR "${CMAKE_SOURCE_DIR}/ThirdPartyLibraries/Windows/sdk")

    set(TEST_DLLS
        "${TEST_THIRD_PARTY_DIR}/libcrypto-3-x64.dll"
        "${TEST_THIRD_PARTY_DIR}/libssl-3-x64.dll"
        "${TEST_THIRD_PARTY_DIR}/libmysql.dll"
        "${TEST_THIRD_PARTY_WIN_SDK_DIR}/iphlpapi.lib"
        "${TEST_THIRD_PARTY_WIN_SDK_DIR}/NetAPI32.Lib"
    )

    set(TEST_QT_MYSQL_PLUGIN "${TEST_THIRD_PARTY_DIR}/sqldrivers/qsqlmysql.dll")

    foreach(TEST_SRC ${TEST_SOURCES})
        get_filename_component(TEST_NAME ${TEST_SRC} NAME_WE)

        # Copy each DLL next to test binary
        foreach(dll ${TEST_DLLS})
            add_custom_command(TARGET ${TEST_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${TEST_SQL_SOURCE}"
                "$<TARGET_FILE_DIR:${TEST_NAME}>/${TEST_SQL_DEST_NAME}"
            )
            add_custom_command(TARGET ${TEST_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${dll}"
                "$<TARGET_FILE_DIR:${TEST_NAME}>"
            )
        endforeach()

        # Copy qsqlmysql plugin to sqldrivers folder
        set(PLUGIN_DEST_DIR "$<TARGET_FILE_DIR:${TEST_NAME}>/sqldrivers")

        add_custom_command(TARGET ${TEST_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${PLUGIN_DEST_DIR}"
        )

        add_custom_command(TARGET ${TEST_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${TEST_QT_MYSQL_PLUGIN}"
            "${PLUGIN_DEST_DIR}"
        )
    endforeach()
endif()

