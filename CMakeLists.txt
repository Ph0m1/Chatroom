cmake_minimum_required(VERSION 3.5)

project(chatroom VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加 Qt 的 moc 处理
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets LinguistTools)
find_package(OpenSSL REQUIRED)

# Set up translation
set(TS_FILES chatroom_zh_CN.ts)
qt_add_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

# Client include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/server/include
    ${CMAKE_SOURCE_DIR}/client/include
    ${CMAKE_SOURCE_DIR}/common
    ${OPENSSL_INCLUDE_DIR}
)

# Client source files
set(CLIENT_SOURCES
    client/core/main.cpp
    client/core/client.cpp
    client/core/widget.cpp
    client/core/log.cpp
    client/core/reg.cpp
    client/core/found.cpp
    client/core/setnewpasswd.cpp
    client/core/settings.cpp
    client/core/menu2.cpp
    client/core/filemenu.cpp
    client/core/creategroupdialog.cpp
    client/core/createfrienddialog.cpp
    client/core/informations.cpp
)

# Client header files
set(CLIENT_HEADERS
    client/include/client.h
    client/include/widget.h
    client/include/log.h
    client/include/reg.h
    client/include/found.h
    client/include/setnewpasswd.h
    client/include/settings.h
    client/include/menu2.h
    client/include/filemenu.h
    client/include/creategroupdialog.h
    client/include/createfrienddialog.h
    client/include/msgtype.h
    client/include/badgetoolbutton.h
    client/include/informations.h
)

# Client common files
set(CLIENT_COMMON
    common/threadpool.h
    common/mysocket.h
    common/mysocket.cpp
)

# Client UI files
set(CLIENT_UI
    client/ui/filemenu.ui
    client/ui/found.ui
    client/ui/informations.ui
    client/ui/log.ui
    client/ui/menu2.ui
    client/ui/reg.ui
    client/ui/setnewpasswd.ui
    client/ui/settings.ui
    client/ui/widget.ui
)

# 使用 qt6/qt5_wrap_ui 来生成 UI 头文件
if(${QT_VERSION_MAJOR} EQUAL 5)
    qt5_wrap_ui(UI_HEADERS ${CLIENT_UI})
elseif(${QT_VERSION_MAJOR} EQUAL 6)
    qt6_wrap_ui(UI_HEADERS ${CLIENT_UI})
endif()

# Resource files
set(RESOURCES
    log.qrc
)

# Combine all client files
set(ALL_CLIENT_FILES
    ${CLIENT_UI}
    ${CLIENT_SOURCES}
    ${CLIENT_HEADERS}
    ${CLIENT_COMMON}
    ${RESOURCES}
    ${TS_FILES}
    ${UI_HEADERS}
)

# Server source files
set(SERVER_SOURCES
    server/server.cpp
    server/db/redis.cpp
    server/smtp/smtpmailer.cpp
    common/mysocket.cpp
)

# Server header files
set(SERVER_HEADERS
    server/include/server.h
    server/include/redis.h
    server/include/smtpmailer.h
    common/mysocket.h
    common/threadpool.h
)

# Combine all server files
set(ALL_SERVER_FILES
    ${SERVER_SOURCES}
    ${SERVER_HEADERS}
)

# Create client executable
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(client
        MANUAL_FINALIZATION
        ${ALL_CLIENT_FILES}
    )
    qt_finalize_executable(client)
else()
    if(ANDROID)
        add_library(client SHARED ${ALL_CLIENT_FILES})
    else()
        add_executable(client ${ALL_CLIENT_FILES})
    endif()
endif()

target_link_libraries(client PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ${OPENSSL_LIBRARIES})

# Create server executable
add_executable(server ${ALL_SERVER_FILES})
target_link_libraries(server PRIVATE ${OPENSSL_LIBRARIES} hiredis curl)

# Install rules
include(GNUInstallDirs)
install(TARGETS client server
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
