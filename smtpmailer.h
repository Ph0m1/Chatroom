#ifndef SMTPMAILER_H
#define SMTPMAILER_H

#include <curl/curl.h>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <cstdint>
#include <cstdlib>      // 用于 std::srand 和 std::rand
#include <ctime>        // 用于 std::time
#include <sstream>      // 用于 std::ostringstream

#define SMTPSERVER "smtp://smtp.qq.com:587"
#define USERNAME "ph0m_hu.xa@foxmail.com"
#define PASSWORD "izgkysbhwhncdcgd"

std::string base64_encode(const std::string& in);

class SMTPMailer {
public:
    // 构造函数，初始化SMTP服务器信息和用户认证信息
    SMTPMailer();

    // 析构函数，清理libcurl资源
    ~SMTPMailer();

    // 发送邮件
    bool sendMail(const std::string& from, const std::vector<std::string>& to,
                  const std::string& subject, const std::string& body);


    // 回调函数，用于提供邮件内容给libcurl
    static size_t payloadSource(void* ptr, size_t size, size_t nmemb, void* userp);

    // 生成邮件内容的payload文本
    std::vector<std::string> generatePayload(const std::string& from, const std::vector<std::string>& to,
                                             const std::string& subject, const std::string& body);

    // 辅助函数，用于连接收件人地址
    std::string join(const std::vector<std::string>& list, const std::string& delimiter) ;


    std::string getcode();  // 生成随机验证码

private:
    std::string smtpServer; // SMTP服务器地址
    std::string username; // 用户名
    std::string password; // 密码
    CURL* curl; // curl会话句柄

    std::string base64Encode(const std::string &input);

    struct upload_status {
        int linesRead = 0; // 已读取的行数
        std::vector<std::string>* payloadText; // 邮件内容的payload文本
    };

};



#endif // SMTPMAILER_H
