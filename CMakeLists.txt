cmake_minimum_required(VERSION 3.5)

project(chatroom VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")
set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)
find_package(OpenSSL REQUIRED)

include_directories(${OPENSSL_INCLUDE_DIR})

set(TS_FILES chatroom_zh_CN.ts)

# Client source files
set(CLIENT_SOURCES
        main.cpp
        widget.cpp
        widget.h
        widget.ui
        ${TS_FILES}
        log.h
        log.cpp
        log.ui
        log.qrc
        threadpool.h
        reg.h
        reg.cpp
        reg.ui
        found.h
        found.cpp
        found.ui
        setnewpasswd.h
        setnewpasswd.cpp
        setnewpasswd.ui
        client.h
        client.cpp
        msgtype.h
        mysocket.h
        mysocket.cpp
        menu2.h
        menu2.cpp
        menu2.ui
        settings.h
        settings.cpp
        settings.ui
        informations.h
        informations.cpp
        informations.ui
        badgetoolbutton.h
        creategroupdialog.h
        creategroupdialog.cpp
        createfrienddialog.h
        createfrienddialog.cpp

        filemenu.h filemenu.cpp filemenu.ui

)

# Sever source files
set(SERVER_SOURCES
        server.cpp
        msgtype.h
        mysocket.h
        mysocket.cpp
        redis.h
        redis.cpp
        threadpool.h
        smtpmailer.h
        smtpmailer.cpp
)

# Client target
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(client
        MANUAL_FINALIZATION
        ${CLIENT_SOURCES}
    )

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(client SHARED
            ${CLIENT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(client
            ${CLIENT_SOURCES}



        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(client PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.chatroom)
endif()

set_target_properties(client PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)
#Server target
add_executable(server ${SERVER_SOURCES}
    server.h)

# Linking libraries for server (add any necessary libraries here)
target_link_libraries(server PRIVATE hiredis curl ${OPENSSL_LIBRARIES})

# Install rules
include(GNUInstallDirs)
install(TARGETS client server
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(client)
endif()
