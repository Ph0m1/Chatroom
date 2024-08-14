#ifndef REDIS_H
#define REDIS_H
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <hiredis/hiredis.h>
#include <iostream>
#include <string>
#include <unordered_map>

#define REDIS_IP "127.0.0.1"
#define REDIS_PORT 6379

class Redis {
private:
    redisContext *m_context;
    redisReply *m_reply;
    std::string m_ip;
    int m_port;
    std::string m_json;

public:
    // 构造函数
    // 连接redis
    Redis() {
        m_ip = REDIS_IP;
        m_port = REDIS_PORT;
        m_context = redisConnect(m_ip.c_str(), m_port);
        if (m_context == NULL || m_context->err) {
            if (m_context) {
                std::cout << "Error: " << m_context->errstr << std::endl;
                redisFree(m_context);
            }
            exit(1);
        }
        // std::cout << "Connect to redis success!" << std::endl;
    }
    // 析构函数
    ~Redis() = default;

    bool Hmset(std::string str,std::string key, std::string value);
    bool Hget(std::string str);
    std::string Hget(std::string key,std::string str);
    bool Hdel(std::string key, std::string str);
    bool Hexists(std::string key, std::string value);
    std::unordered_map<std::string,std::string> Hmget(std::string key);

    bool Sadd(std::string str,std::string value);// 向集合添加一个或多个成员
    bool Scard(int key);
    bool Srem(std::string key, std::string str);
    bool Sscan(int key, std::string str);
    bool Sismember( std::string str,std::string value);
    std::vector<std::string> Smembers(std::string msg);
    bool Del(std::string str);
};

#endif // REDIS_H
