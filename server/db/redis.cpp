#include "redis.h"

bool Redis::Hmset(std::string str, std::string key, std::string value) {
    m_reply = (redisReply *)redisCommand(m_context, "HMSET %s %s %s", str.c_str(), key.c_str(), value.c_str());
    if (m_reply->type == REDIS_REPLY_ERROR) {
        std::cout << "Error: " << m_reply->str << std::endl;
        return false;
    }
    return true;
}
bool Redis::Hget(std::string str){
    std::string cmd = "HGET " + str;
    m_reply = (redisReply *)redisCommand(m_context, cmd.c_str());
    if (m_reply->type == REDIS_REPLY_ERROR) {
        std::cout << "Error: " << m_reply->str << std::endl;
        return false;
    }
    if (m_reply->str == nullptr){
        return false;
    }
    return true;
}
std::string Redis::Hget(std::string key,std::string str) {
    m_reply = (redisReply *)redisCommand(m_context, "HGET %s %s",key.c_str(),str.c_str());
    if (m_reply->type == REDIS_REPLY_ERROR) {
        std::cout << "Error: " << m_reply->str << std::endl;
        return " ";
    }
    if(m_reply->type == REDIS_REPLY_NIL){
        return " ";
    }
    return m_reply->str;
}
bool Redis::Hdel(std::string key, std::string str) {
    m_reply = (redisReply *)redisCommand(m_context, "HDEL %s %s", key.c_str(), str.c_str());
    if (m_reply->type == REDIS_REPLY_ERROR) {
        std::cout << "Error: " << m_reply->str << std::endl;
        return false;
    }
    return true;
}
bool Redis::Hexists(std::string key, std::string value) {

    // std::string cmd = "HEXISTS " + key + " " + value;
    m_reply = (redisReply *)redisCommand(m_context, "HEXISTS %s %s",key.c_str(),value.c_str());
    if (m_reply->type == REDIS_REPLY_ERROR) {
        std::cout << "Error: " << m_reply->str << std::endl;
        return false;
    }
     bool exists = m_reply->integer == 1;
    // freeReplyObject()
    return exists;
}
std::unordered_map<std::string,std::string> Redis::Hmget(std::string key) {
    std::string cmd = "HGETALL " + key;
    std::unordered_map<std::string,std::string> a;
    m_reply = (redisReply *)redisCommand(m_context, cmd.c_str());
    if (m_reply->type == REDIS_REPLY_ARRAY) {
        for(size_t i= 0;i< m_reply->elements;i+=2){
            a[m_reply->element[i]->str] = m_reply->element[i+1]->str;
        }
    }

    return a;
}

bool Redis::Sadd(std::string str,std::string value){// 向集合添加成员
    redisReply *reply = (redisReply *)redisCommand(m_context, "SADD %s %s",str.c_str(),value.c_str());
    if (reply == nullptr) {
        std::cerr << "Redis SADD command failed." << std::endl;
        return false;
    }
    bool result = reply->integer > 0;
    freeReplyObject(reply);
    return result;
}

bool Redis::Del(std::string str){
    redisReply *reply = (redisReply*) redisCommand(m_context,"DEL %s",str.c_str());
    if(reply->type==REDIS_REPLY_ERROR){
        freeReplyObject(reply);
        return false;
    }
    freeReplyObject(reply);
    return true;
}
bool Scard(int key);
bool Redis::Srem(std::string key, std::string str){
    std::string cmd = "SREM "+ key + " " + str;
    redisReply *reply = (redisReply*) redisCommand(m_context,cmd.c_str());
    if(reply == nullptr ){
        std::cerr<<"Redis SREM command failed." <<std::endl;
        return false;
    }
    return reply->integer;
}
bool Sscan(int key, std::string str);
bool Redis::Sismember( std::string str,std::string value){
    redisReply *reply = (redisReply*)redisCommand(m_context,"SISMEMBER %s %s",str.c_str(),value.c_str());
    if(reply == nullptr){
        std::cerr << "Redis SISMEMBER command failed." << std::endl;
        return false;
    }
    bool result = reply->integer == 1;
    freeReplyObject(reply);
    return result;
}
std::vector<std::string> Redis::Smembers(std::string str){
    redisReply *reply = (redisReply*)redisCommand(m_context, "SMEMBERS %s",str.c_str());
    std::vector<std::string> vector;
    if(reply->type == REDIS_REPLY_ARRAY){
        for(size_t i = 0; i < reply->elements; ++i){
            vector.push_back(reply->element[i]->str);
        }
    }else{
        std::cerr << "Unexpected reply type: " << reply->type << std::endl;
    }
    return vector;

}
