#ifndef SERVER_H
#define SERVER_H
#include "redis.h"
#include "threadpool.h"
#include "mysocket.h"
#include "msgtype.h"
#include <openssl/evp.h>
#include <curl/curl.h>
#include <unordered_map>
#include <cstring>
#include <sstream>
#include <string>
#include <iomanip>
#include <filesystem>

#define UsernameHash "UsernameSet"
#define UserInfo "UserInfo"
#define UidSet "UidSet"
#define EmailHash "EmailHash"
#define OnlineSet "OnlineSet"


class Server {
private:
    int server_fd;
    struct sockaddr_in server_addr;
    std::unordered_map<int, struct sockaddr_in> clients;
    ThreadPool threadPool;
    int epoll_fd;
    struct epoll_event ev, events[MAX_LINK];

    // 在线列表
    std::unordered_map<std::string, int> onlinelist;
    // 绑定fd和用户
    std::unordered_map<int, std::string> users;
    // 客户端状态
    std::unordered_map<int, int> status;
    // 绑定fd和验证码
    std::unordered_map<int,std::string> capts;
    // 设置找回队列
    std::unordered_map<int ,std::string> found_queue;

    std::mutex pauseMutex;
    std::condition_variable pauseCondition;
    bool pauseThread = false;
private:
    void pause();
    void resume();

    void handleClient(int client_fd);

    void handleMessage(int client_fd, MsgType type, const std::string &message);

    void broadcastMessage(int fd, const std::string &message);

    std::string sha256(const std::string& str);


    // 处理报文的函数
    void reg(int fd, std::string str);
    void login(int fd,std::string str);
    void foundAccount(int fd,std::string str);
    void infoReset(int fd,std::string str);
    void deleteAccount(int fd,std::string str);
    void resetpasswd(int fd,std::string str);
    void createGroup(int fd, std::string str);
    void joinGroup(int fd, std::string str);
    void exitGroup(int fd, std::string str);

    void enter(int fd, std::string str);
    void left(int fd, std::string str);

    void Message(int fd, std::string str);
    void files(int fd, std::string str);

    void addFriend(int fd, std::string str);
    void acceptAddFrined(int fd, std::string str);
    void refuseAddFriend(int fd, std::string msg);

    void deleteFriend(int fd, std::string str);

    void bannedFriend(int fd, std::string str);
    void accountInit( std::string str);
    void refreshFriendlist(int fd, std::string str);

    void acceptfile(int fd, std::string str);
    std::unordered_map<std::string, std::string> regetFriendlist(std::string id);

    void logout(std::string id);
    // 校验验证码
    void captcha(int fd, std::string str);
    // 获取用户名
    std::string getusername(std::string uid);

    // 生成uid
    std::string generateUid();
    // 设置为非阻塞
    int setNonBlocking(int fd) {
        int flags = fcntl(fd, F_GETFL, 0);
        if (flags == -1) {
            return -1;
        }
        return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    }

    void addmember(std::string id, std::string gid, std::string gname);
public:


    Server() : threadPool(10) {}

    void init() {

        // Redis redis;

        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd == -1) {
            perror("socket");
            exit(EXIT_FAILURE);
        }

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(S_PORT);
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
            perror("bind");
            exit(EXIT_FAILURE);
        }

        if (listen(server_fd, MAX_LINK) == -1) {
            perror("listen");
            exit(EXIT_FAILURE);
        }

        epoll_fd = epoll_create1(0);
        if (epoll_fd == -1) {
            perror("epoll_create1");
            exit(EXIT_FAILURE);
        }

        ev.events = EPOLLIN;
        ev.data.fd = server_fd;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) == -1) {
            perror("epoll_ctl: server_fd");
            exit(EXIT_FAILURE);
        }

        threadPool.init();
        std::cout << "Chat server initialized." << std::endl;

    }

    void run() {
        while (true) {
            int nfds = epoll_wait(epoll_fd, events, MAX_LINK, -1);
            if (nfds == -1) {
                perror("epoll_wait");
                exit(EXIT_FAILURE);
            }

            for (int n = 0; n < nfds; ++n) {
                if(events[n].events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)){
                    int client_fd = events[n].data.fd;
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                    close(client_fd);
                    clients.erase(client_fd);
                    std::cout << "Client disconnected: " << client_fd << std::endl;
                }
                if (events[n].data.fd == server_fd) {
                    struct sockaddr_in client_addr;
                    socklen_t client_len = sizeof(client_addr);
                    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
                    if (client_fd == -1) {
                        perror("accept");
                        continue;
                    }
                    std::cout << "\033[41m1567890898"<< " \033[0m";
                    clients[client_fd] = client_addr;
                    setNonBlocking(client_fd);
                    ev.events = EPOLLIN | EPOLLET;
                    ev.data.fd = client_fd;
                    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1) {
                        perror("epoll_ctl: client_fd");
                        exit(EXIT_FAILURE);
                    }

                    std::cout << "Client connected: " << client_fd << std::endl;
                } else {
                    int client_fd = events[n].data.fd;
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                    threadPool.submit([this, client_fd]() {

                        std::cout<<client_fd<<std::endl;
                        // int count = getdtablesize() - close(0);
                        // std::cout << "Current open file descriptors: " << count << std::endl;
                        // std::unique_lock<std::mutex> lock(pauseMutex);
                        // pauseCondition.wait(lock, [this] { return !pauseThread; });
                        handleClient(client_fd);
                    });

                }
            }
        }
    }

    ~Server() {
        close(server_fd);
        close(epoll_fd);
    }
};

void sendFriendsList(int fd,std::string id);
std::unordered_map<std::string,std::string> getFl(const std::string &key);
std::unordered_map<std::string,std::string> getGl(const std::string &key);
std::unordered_map<std::string,std::string> getMl(const std::string &key);

#endif // SERVER_H
