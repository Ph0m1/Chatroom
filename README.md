# Ph0m's Chatroom

## 介绍

## 项目依赖

    - hiredis
    - openssl
    - Qt6
    - Qt5

## 使用方法

- 下载源码
    
```shell
        git clone https://github.com/Ph0m1/Chatroom.git
        cd Chatroom
```
    
- 安装依赖
    
archlinux：

```shell
    sudo pacman -S hiredis openssl qt5 qt6 nlohmann-json
```

ubuntu：
    
```sh
    sudo apt install libhiredis-dev libssl-dev qt6-base-dev qtbase5-dev qttools5-dev-tools qttools5-dev qt6-tools-dev qt6-tools-dev-tools qt6-linguist-tools nlohmann-json-dev
```

- 快速编译运行

```shell
    ./server_run.sh
    ./client_run.sh
```
- 编译运行

```shell
    mkdir build && cd build
    cmake .. 
```
只编译客户端
```shell
    make client -j
    ./client
```
只编译服务端
```shell
    make server -j
    ./server
```

## 项目结构

.
├── chatroom_zh_CN.ts
├── client
│   ├── core
│   │   ├── client.cpp
│   │   ├── createfrienddialog.cpp
│   │   ├── creategroupdialog.cpp
│   │   ├── filemenu.cpp
│   │   ├── filemenu.ui
│   │   ├── found.cpp
│   │   ├── found.ui
│   │   ├── informations.cpp
│   │   ├── informations.ui
│   │   ├── log.cpp
│   │   ├── log.ui
│   │   ├── main.cpp
│   │   ├── menu2.cpp
│   │   ├── menu2.ui
│   │   ├── reg.cpp
│   │   ├── reg.ui
│   │   ├── setnewpasswd.cpp
│   │   ├── setnewpasswd.ui
│   │   ├── settings.cpp
│   │   ├── settings.ui
│   │   ├── widget.cpp
│   │   └── widget.ui
│   ├── include
│   │   ├── badgetoolbutton.h
│   │   ├── client.h
│   │   ├── createfrienddialog.h
│   │   ├── creategroupdialog.h
│   │   ├── filemenu.h
│   │   ├── found.h
│   │   ├── informations.h
│   │   ├── log.h
│   │   ├── menu2.h
│   │   ├── msgtype.h
│   │   ├── reg.h
│   │   ├── setnewpasswd.h
│   │   ├── settings.h
│   │   └── widget.h
│   └── ui
│       ├── filemenu.ui
│       ├── found.ui
│       ├── informations.ui
│       ├── log.ui
│       ├── menu2.ui
│       ├── reg.ui
│       ├── setnewpasswd.ui
│       ├── settings.ui
│       └── widget.ui
├── common
│   ├── mysocket.cpp
│   ├── mysocket.h
│   └── threadpool.h
├── Header.jpeg
├── images
│   ├── add.png
│   ├── banned.png
│   ├── chat.png
│   ├── chatroom_zh_CN.ts
│   ├── clear.png
│   ├── file.png
│   ├── gear.png
│   ├── italic.png
│   ├── magnifying.png
│   ├── remove.png
│   ├── signout.png
│   ├── strong.png
│   ├── underline.png
│   └── user.png
│   ├── db
│   │   └── redis.cpp
│   ├── include
│   │   ├── redis.h
│   │   ├── server.h
│   │   └── smtpmailer.h
│   ├── server.cpp
│   └── smtp
│       └── smtpmailer.cpp
├── log.qrc
├── README.md
├── server
├── client_run.sh
├── CMakeLists.txt
├── CMakeLists.txt.user
└── server_run.sh
