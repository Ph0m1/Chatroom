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

