#include "redis.h"
#include "mysocket.h"
#include "msgtype.h"
#include "server.h"
#include "smtpmailer.h"
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
using json = nlohmann::json;
int main() {
    Server server;
    server.init();
    server.run();
    return 0;
}


void Server::handleClient(int client_fd) {
        try{
        std::string message;
        MsgType msgType = recvMsg(client_fd, message);
        if (msgType == Failure || msgType == Disconnent) {
            // 客户端断开连接
            onlinelist.erase(users[client_fd]);
            users.erase(client_fd);
            close(client_fd);
            std::cout<<message<<std::endl;
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
            clients.erase(client_fd);
            std::cout << "Client disconnected: " << client_fd << std::endl; 
        } else {
            // 处理收到的消息
            handleMessage(client_fd, msgType, message);
            ev.events = EPOLLIN | EPOLLET;
            ev.data.fd = client_fd;
            if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1) {
                perror("11epoll_ctl: client_fd");
            }
        }
        }catch(const std::exception &e){
            std::cerr << "Expection in handleClient: " << e.what() << std::endl;
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
            close(client_fd);
        }
    }

void Server::handleMessage(int fd, MsgType type, const std::string &msg) {
        // 根据消息类型进行处理
        switch (type) {
        case Disconnent:
            // 客户端断开连接
            onlinelist.erase(users[fd]);
            users.erase(fd);
            close(fd);
            std::cout<<msg<<std::endl;
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
            clients.erase(fd);
            if(onlinelist.find(users[fd]) != onlinelist.end()){
                onlinelist.erase(users[fd]);
            }
            users.erase(fd);
            std::cout << "Client disconnected: " << fd << std::endl;
            break;
        case UserAccount:
            std::cout << "User Register message:" << msg << std::endl;
            reg(fd,msg);
            break;
        case UserLogin:
            std::cout << "User login message: " << msg << std::endl;
            login(fd,msg);
            break;
        case OutLine:
            std::cout << "User logout : " << msg << std::endl;
            logout(msg);
            break;
        case AccountFound:
            std::cout << "User found message: " << msg << std::endl;
            foundAccount(fd,msg);
            break;
        case Captcha:
            captcha(fd,msg);
            break;
        case ResetPasswd:
            resetpasswd(fd,msg);
            break;
        // case ResetInfo:
        //     std::cout << "User reset infomation message: " << msg << std::endl;
        //     infoReset(fd,msg);
        //     break;
        case AccountDelete:
            std::cout << "User delete account: " << msg << std::endl;
            deleteAccount(fd,msg);
            break;
        case GroupCreat:
            std::cout << "User create group message: " << msg << std::endl;
            createGroup(fd,msg);
            break;
        case HistoryMsg:
            sendHistoryMsg(fd, msg);
            break;
        // case GroupJoin:
        //     joinGroup(fd,msg);
        //     break;
        // case GroupExit:
        //     exitGroup(fd,msg);
        //     break;
        // case UserEnter:
        //     enter(fd,msg);
        //     break;
        // case UserLeft:
        //     left(fd,msg);
        //     break;
        case Msg:
            Message(fd,msg);
            break;
        case File://fd
            files(fd,msg);
            break;
        case AcceptFiles:
            acceptfile(fd, msg);
            break;
        case FriendAdd:
            std::cout << "User add frined: "<< msg << std::endl;
            addFriend(fd,msg);
            break;
        case FriendDelete:
            deleteFriend(fd,msg);
            break;
        case FriendBanned:
            bannedFriend(fd,msg);
            break;
        case FriendAddYes:
            acceptAddFrined(fd,msg);
            break;
        case FriendAddNo:
            refuseAddFriend(fd, msg);
            break;
        case GroupJoinYes:
            break;
        case GroupJoinNo:
            break;
        default:
            std::cout << "Unknown message type: " << type << std::endl;
            break;
        }
    }

void Server::sendHistoryMsg(int fd, std::string msg){
    Json js;
    js["Msges"] = getMl(msg);
    std::string data = js.dump();
    size_t size = data.size();
    Json s;
    s["Size"] = size;

    // int flags = fcntl(fd, F_GETFL, 0);
    // if(flags == -1){
    //     std::cerr << "fcntl(F_GETFL) failed"<< std::endl;
    //     return;
    // }

    // flags &= ~O_NONBLOCK;

    // if(fcntl(fd, F_SETFL, flags) == -1){
    //     std::cerr << "fcntl(F_SETFL) failed"<< std::endl;
    //     return;
    // }

    sendMsg(fd, HistoryMsg, s.dump());
    const char* p = data.c_str();
    // size_t sent_size = 0;
    ssize_t sent_bytes;
    char buffer[5242880];
    size_t len;
    while(size > 0){
        len = size < sizeof(buffer) ? size : sizeof(buffer);
        sent_bytes = send(fd, p, len, 0);
        if(sent_bytes <= -1){
            if(errno == EINTR && sent_bytes == -1){
                continue;
            }
            else
            {
                std::cerr << "Error sending data: "<<strerror(errno) << std::endl;
                return;
            }
        }
        else if(sent_bytes == 0){
            continue;
        }
        p += sent_bytes;
        size -= sent_bytes;
    }

    // if(fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1){
    //     std::cerr << "fcntl(F_SETFL) failed"<< std::endl;
    //     return;
    // }

}

void Server::deleteFriend(int fd, std::string msg){
    Json js = Json::parse(msg.data());
    std::vector<std::string> friends = js["Info"].get<std::vector<std::string>>();
    std::string m_id = users[fd];
    Redis r;
    for(auto &u_id : friends){
        // 从本人好友列表中删除对方
        r.Hdel(m_id + "f", u_id);
        // 从对方好友列表中删除本人
        r.Hdel(u_id + "f", m_id);

        if(onlinelist.find(u_id) != onlinelist.end()){
            std::unordered_map<std::string, std::string> fl = getFl(u_id);
            Json temp = {"FriendList",fl};
            std::string str = temp.dump();
            sendMsg(onlinelist[u_id], FriendDelete, str);
        }
    }
}


void Server::broadcastMessage(int fd, const std::string &message) {
    Json js = Json::parse(message);
    std::string msg = js["Msg"];
    std::string sender = msg.substr(0,9);
    std::string gid = msg.substr(9,9);
    Redis r;
    std::vector<std::string> members = r.Smembers(gid+"member");
    for(auto & member : members){
        std::string recver = member.substr(0,9);
        std::string newmsg = gid +sender + msg.substr(18);
        Json msg;
        msg["Msg"] = newmsg;
        msg["Time"] = js["Time"].get<std::string>();
        // msg["Status"] = Unread;
        std::string data;
            // 跳过广播给发送者
        if(onlinelist.find(recver) != onlinelist.end()){
            msg["Status"] = Readen;
            data = msg.dump();
            if(recver == sender){
                r.Hmset(sender + "m", sha256(data), data);
                continue;
            }
            sendMsg(onlinelist[recver], Msg, data);
        }else{
            msg["Status"] = Unread;
            data = msg.dump();
        }
        r.Hmset(recver + "m", sha256(data), data);
    }
}

// 生成uid
std::string Server::generateUid(){
        std::srand(static_cast<unsigned int>(std::time(0)));
        std::ostringstream idStream;
        int a = rand() % 9 + 1;
        idStream<<a;
        for (int i = 1; i < 9; ++i) {
            int digit = rand() % 10; // 生成0到9之间的随机数
            idStream << digit;
        }
        Redis redis;
        std::string b = "UidSet";
        if(redis.Sismember(b,idStream.str())){
            return generateUid();
        }
        return idStream.str();
}



void Server::logout(std::string id){
    users.erase(onlinelist[id]);
    onlinelist.erase(id);
}

void Server::reg(int fd, std::string str){
    std::string uid = generateUid();
    nlohmann::json js = nlohmann::json::parse(str);
    std::string name = js["username"];
    std::string mail = js["mail"];

    Redis redis;
    if(redis.Hget(UsernameHash, name) != " "){
        sendMsg(fd,Refuse,"该用户名已注册");
        return;
    }else if(redis.Hget(EmailHash, mail) != " "){
        sendMsg(fd,Refuse,"该邮箱已注册");
        return;
    }
    redis.Hmset(UsernameHash, name, uid);
    redis.Hmset("Userid", uid, name);
    redis.Hmset(EmailHash, mail, uid);
    redis.Hmset(UserInfo, uid, str);

    redis.Sadd(UidSet,uid);

    accountInit(uid);

    sendMsg(fd,Success,uid);
}
void Server::login(int fd,std::string str){
    nlohmann::json js = nlohmann::json::parse(str.data());

    std::string id = js["id"].get<std::string>();
    std::string passwd = js["passwd"].get<std::string>();
    Redis r;
    std::string uid = r.Hget(EmailHash,id);

    std::cout<<uid<<std::endl;
    if(uid != " "){
        id = uid;
    }
    std::string info = r.Hget(UserInfo,id);
    if (info == " "){
        sendMsg(fd,Refuse,"该账户不存在");
    }
    js = nlohmann::json::parse(r.Hget(UserInfo,id));
    if(!(js["passwd"] == passwd)){
        sendMsg(fd,Refuse,"帐号或密码错误");
        return;
    }
    if(onlinelist.find(id) != onlinelist.end()){
        sendMsg(fd,Refuse,"帐号已登陆");
    }

    users[fd] = id;
    //发送个人信息

    //json
    // Info: json
    // Msglist: map
    // Friendlist: map
    // Gruoplist: map
    json rec;
    rec["Uid"] = id;
    rec["FriendList"] = getFl(id);
    // 获取群聊信息
    std::unordered_map<std::string, std::string> gl = getGl(id);
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> gmembers;
    for(auto &t : gl ){
        Redis r;
        std::vector<std::string> ll =  r.Smembers(t.first + "member");
        std::unordered_map<std::string, std::string> ss;
        for(auto& s : ll){
            ss[s.substr(0,9)] = r.Hget("Userid", s.substr(0,9));
        }
        gmembers[t.first] = ss;
    }
    rec["GroupList"] = gl;
    rec["GroupMembers"] =  gmembers;

    rec["Info"] = js;
    rec["FriendAdd"] = r.Smembers(id+"000");
    std::string data = rec.dump();
    sendMsg(fd,Success,data);
    onlinelist[id] = fd;
    {
        for(auto &t : onlinelist){
            std::cout << t.first << " : " << t.second << std::endl;
        }

    }
    users[fd] = id;
}
void Server::foundAccount(int fd,std::string str){
    SMTPMailer mailer;
    std::string from = "<ph0m_hu.xa@foxmail.com>";
    std::vector<std::string> to = {"<" + str + ">"};
    std::string subject = "验证码";
    std::string c = mailer.getcode();
    std::string body = "您的验证码是: " + c;
    capts[fd] = c;
    if(mailer.sendMail(from, to, subject,body)){
        sendMsg(fd,Success);
    }else {
        sendMsg(fd,MailError);
    }
    found_queue[fd] = str;
}
void Server::accountInit(std::string str){
    std::cout<<"正在初始化账户"<<std::endl;
    std::string superid = "000000001";
    Redis r;

    std::string str1 =superid + str + "欢迎来到PH0M的聊天室!";
    Json js;
    {
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);

        std::tm* now_tm = std::localtime(&now_c);

        std::ostringstream oss;
        oss << std::put_time(now_tm, "%Y-%m-%d[%H:%M:%S]");
        js["Time"] = oss.str();
    }
    js["Msg"] = str1;
    std::string dt = js.dump();
    r.Hmset(str+"m",sha256(dt), dt);

    r.Hmset(str+ "f", superid, "小H");

    std::cout<<"账户初始化完成"<<std::endl;
}
void Server::captcha(int fd,std::string buf){

    if (buf == capts[fd]){
        if(users[fd].empty()){
           sendMsg(fd,Success,"12");
        }
        sendMsg(fd,Success,users[fd]);
    }
    else{
        sendMsg(fd,Refuse,"111");
        return;
    }
}

void Server::resetpasswd(int fd, std::string str){
    Redis r;
    std::string mail = found_queue[fd];
    std::string id = r.Hget(EmailHash,mail);
    std::cout<<id<<std::endl;
    json infojs = json::parse(r.Hget(UserInfo,id).c_str());
    infojs["passwd"] = str;

    std::string a = infojs.dump();
    std::cout<<a<<std::endl;
    r.Hmset(UserInfo, id, a);
    found_queue.erase(fd);
    sendMsg(fd,Success,"11");
}

void Server::infoReset(int fd,std::string str){

}
void Server::deleteAccount(int fd,std::string str){
    Redis r;
    std::string Info;
    Info = r.Hget(UserInfo,str);
    std::cout<<"111"<<std::endl;
    if(Info == " "){
        sendMsg(fd,Refuse);
        return;
    }
    std::cout<<"2222"<<std::endl;
    json ijs = json::parse(Info.data());
    std::string username = ijs["username"].get<std::string>();
    std::string mail = ijs["mail"].get<std::string>();
    std::string id = r.Hget(EmailHash,mail);

    r.Hdel(EmailHash,mail);
    r.Hdel(UserInfo,id);
    r.Srem(UidSet,id);
    r.Hdel(UsernameHash,username);
    r.Hdel("Userid", id);
    // ...
    // 删除信息
    std::unordered_map<std::string, std::string> fl = r.Hmget(id + "f");
    for(auto &t : fl){
        r.Hdel(t.first+"f",id);
        if(onlinelist.find(t.first) != onlinelist.end()){
            Json js;
            js["Time"]="00000000";
            js["Msg"] = id+"000000000该用户已注销！";
            js["Status"] = Readen;
            std::string data = js.dump();
            sendMsg(onlinelist[t.first], Msg, data);
        }
    }
    r.Del(id+"f");
    r.Del(id+"m");
    r.Del(id+"000");
}

// 创建群聊
void Server::createGroup(int fd, std::string str){
    Json js =  Json::parse(str.data());
    Redis r;
     // 分配群聊ID
    std::string gid = generateUid();
    r.Sadd("UidSet", gid);

    std::string owner = js["Owner"].get<std::string>();
    std::string gname = js["Gname"].get<std::string>();
    std::vector<std::string> friends = js["Members"].get<std::vector<std::string>>();
    // 添加群聊信息
    std::cout<<"||||------||||||||"<<std::endl;
    Json info;
    info["Owner"] = js["Owner"].get<std::string>();
    info["Gid"] = gid;
    info["Gname"] = gname;
    // 添加群主 2
    Json ret;
    ret["Uid"] = gid;
    ret["Uname"] = gname;
    ret["Members"] = friends;
    r.Sadd(gid + "member", owner + "2");
    r.Hmset(owner+"g",gid,gname);
    // 添加普通成员 0
    for(auto& t : friends){
        addmember(t, gid, gname);
        if(onlinelist.find(t) != onlinelist.end()){

            sendMsg(onlinelist[t], ReFreshGroupList, ret.dump());
        }

        sendMsg(fd, ReFreshGroupList, ret.dump());

    }
    r.Hmset("GroupInfo", gid, info.dump());
    r.Hmset("Groupid", gid, gname);
    r.Hmset("GroupName", gname, gid);
}

void Server::addmember(std::string id, std::string gid, std::string gname){
    std::string key = gid + "member";
    Redis r;
    r.Sadd(key, id+"0");
    r.Hmset(id + "g", gid, gname);
}
void Server::joinGroup(int fd, std::string str){
    Redis r;
    if(r.Hexists(users[fd] + "g", str)){
        sendMsg(fd, GroupJoinNo, "你已加入该群聊！");
    }
    // 初始按照普通用户存储
    r.Sadd(str + "member", users[fd] + "0");
    std::string s = r.Hget("Groupid",str);
    r.Hmset(users[fd] + "g" , str, s);
}
void acceptJoinGroup();
void exitGroup(int fd, std::string str);

void enter(int fd, std::string str);
void left(int fd, std::string str);

void Server::Message(int fd, std::string str){
    Json js = json::parse(str);
    std::string time = js["Time"].get<std::string>();
    std::string msg = js["Msg"].get<std::string>();

    std::string sender = msg.substr(0,9);
    std::string recver = msg.substr(9,9);
    std::string data = js.dump();
    std::string hash = sha256(str);
    Redis r;
    if(r.Sismember(recver + "111", sender)){
        Json ret;
        ret["Time"] = js["Time"].get<std::string>();
        ret["Msg"] = recver + "000000000" + "发送失败，对方已开启好友验证";
        // std::string d = ret.dump();
        ret["Status"] = Readen;
        sendMsg(fd, Msg, ret.dump());
        return;
    }
    if(r.Hexists("GroupInfo", recver)){
        std::cout<< "[SSSSSSSS]"<<std::endl;
        if(r.Sismember(recver + "222", sender)){
            Json ret;
            ret["Time"] = js["Time"].get<std::string>();
            ret["Msg"] = recver + "000000000" + "发送失败，你已被管理员禁言";
            ret["Status"] = Readen;
            sendMsg(fd, Msg, ret.dump());
            return;
        }
        broadcastMessage(fd, str);
        return;
    }
    if((!r.Hexists(sender + "f", recver)) && !r.Hexists(recver + "f", sender)){
        std::cout<< "[FFFFFFFF]"<<std::endl;
        Json ret;
        ret["Time"] = js["Time"].get<std::string>();
        ret["Msg"] = recver + "000000000" + "发送失败，你还不是对方好友";
        ret["Status"] = Readen;
        std::string d = ret.dump();
        sendMsg(onlinelist[sender], Msg, d);
        return;
    }
    if(onlinelist.find(recver) != onlinelist.end()){
        std::cout<< "[AdDADADA]"<<std::endl;
        sendMsg(onlinelist[recver],Msg,data);
        js["Status"] = Readen;
    }
    std::cout<< "[aaaaaaaaa]"<<std::endl;
    r.Hmset(sender + "m", hash, data);
    r.Hmset(recver + "m", hash, data);

}
// Hash函数
std::string Server::sha256(const std::string& str) {
    EVP_MD_CTX* mdctx = EVP_MD_CTX_new(); //创建一个新的 EVP_MD_CTX 对象
    if(mdctx == nullptr){
        throw std::runtime_error("Failed to create EVP_MD_CTX");
    }
    if(1 != EVP_DigestInit_ex(mdctx, EVP_sha256(), nullptr))
    {
        EVP_MD_CTX_free(mdctx);
        throw std::runtime_error("Failed to initialize EVP_DigestInit_ex");
    }
    // 更新哈希上下文，传递要哈希的字符串
    if(1 != EVP_DigestUpdate(mdctx, str.c_str(), str.length()))
    {
        EVP_MD_CTX_free(mdctx);
        throw std::runtime_error("Failed to update EVP_DigestUpdate");
    }

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int len = 0;
    // 完成哈希计算并获取结果
    if(1 != EVP_DigestFinal_ex(mdctx, hash, &len)){
        EVP_MD_CTX_free(mdctx);
        throw std::runtime_error("Failed to finalize digest");
    }
    //  释放上下文
    EVP_MD_CTX_free(mdctx);
    // 将哈希结果转换为十六进制字符串
    std::stringstream ss;
    for(unsigned int i = 0; i < len; ++i){
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);    
    }
    return ss.str();
}
void Server::files(int fd, std::string str){
    int flags = fcntl(fd, F_GETFL, 0);
    if(flags == -1){
        std::cerr << "fcntl(F_GETFL) failed"<< std::endl;
        return;
    }

    flags &= ~O_NONBLOCK;

    if(fcntl(fd, F_SETFL, flags) == -1){
        std::cerr << "fcntl(F_SETFL) failed"<< std::endl;
        return;
    }

    Json fileinfo = Json::parse(str);
    size_t size = fileinfo["Size"].get<size_t>();
    std::string filename = fileinfo["Filename"].get<std::string>();
    recvFile(fd, size, filename, SERVER_FILES);
    Redis r;
    std::string uid = fileinfo["To"].get<std::string>();
    std::string mid = fileinfo["From"].get<std::string>();
    // fileinfo.erase("To");
    // fileinfo["From"] = users[fd];
    fileinfo["Flag"] = 1;
    std::string filedata = fileinfo.dump();
    r.Sadd(uid+"files", filedata);

    // std::filesystem::path currentpath = std::filesystem::current_path();
    // std::string filepath = (currentpath / SERVER_FILES / filename).c_str();
    // r.Hmset("Filepath",filename,filepath);
    if(onlinelist.find(uid) != onlinelist.end()){
        sendMsg(onlinelist[uid], FileInfo, filedata);
    }

    if(r.Hexists("GroupInfo",uid)){
        fileinfo["Flag"] = 2;
        std::vector<std::string> members = r.Smembers(uid + "member");
        for(auto &member : members){
            std::string id = member.substr(0,9);
            if(onlinelist.find(uid) != onlinelist.end() ){
                sendMsg(onlinelist[uid], FileInfo, fileinfo.dump());
            }
        }
    }


    if(fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1){
        std::cerr << "fcntl(F_SETFL) failed"<< std::endl;
        return;
    }
    // epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
    // clients.erase(fd);
    std::cout << "[FINSHED WORK] " << std::endl;
}

void Server::acceptfile(int fd, std::string fileinfo){
    int flags = fcntl(fd, F_GETFL, 0);
    if(flags == -1){
        std::cerr << "fcntl(F_GETFL) failed"<< std::endl;
        return;
    }

    flags &= ~O_NONBLOCK;

    if(fcntl(fd, F_SETFL, flags) == -1){
        std::cerr << "fcntl(F_SETFL) failed"<< std::endl;
        return;
    }
    std::string filename = fileinfo.substr(18);
    std::filesystem::path currentpath = std::filesystem::current_path();
    std::string filepath = currentpath / SERVER_FILES / filename;
    ::sendFile(fd, filepath, fileinfo.substr(9,9), fileinfo.substr(0,9));
    // 删除服务器本地文件
    if(fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1){
        std::cerr << "fcntl(F_SETFL) failed"<< std::endl;
        return;
    }
    // close(fd);
    // epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
    // clients.erase(fd);
    std::cout << "[FINSHED WORK] " << std::endl;
    // std::remove(filepath.c_str());
}

void Server::addFriend(int fd, std::string str){
    Redis r;
    std::string uid = r.Hget(EmailHash,str);
    std::string mid = users[fd];
    std::string uname = getusername(uid);
    std::string mname = getusername(mid);
    std::cout << str.c_str() <<"  " <<uid.c_str() << std::endl;
    if(uid == " "){
        uid = str;
    }
    if(!r.Sismember("UidSet",uid) || uid == " "){
        Json js;
        js["Msg"] = "该用户/群聊不存在";
        js["Status"] = Failure;
        std::string data = js.dump();
        sendMsg(fd,FriendAdd,data);
        return;
    }
    if(r.Hexists("GroupInfo", uid)){
        if(r.Hexists(mid + "g", uid)){
            Json js;
            js["Msg"] = "你已在群聊中";
            js["Status"] = Failure;
            std::string data = js.dump();
            sendMsg(fd,FriendAdd,data);
            return;
        }
        std::string uname = r.Hget("Groupid", uid);
        std::vector<std::string> members = r.Smembers(uid+"member");
        r.Sadd(uid + "0000", mid);
        Json js;
        js["Msg"] = "已向群聊：" + uname +"<" + uid + "> 发送入群申请！";
        js["Status"] = Success;
        std::string data = js.dump();
        sendMsg(fd,FriendAdd,data);
        for(auto & member : members){
            if(member.substr(9,1) == "0"){
                continue;
            }
            r.Sadd(member.substr(0,9) + "0000", uid+mid);
            if(onlinelist.find(member.substr(0,9)) != onlinelist.end()){
                sendMsg(onlinelist[member.substr(0,9)], GroupJoin, mid);
            }
        }
    }



    if(mid == uid){
        Json js;
        js["Msg"] = "请勿添加本人为好友";
        js["Status"] = Failure;
        sendMsg(fd, FriendAdd, js.dump());
        return;
    }
    if(r.Hexists(mid + "f", uid)){
        Json js;
        js["Msg"] = "该用户已是您的好友";
        js["Status"] = Failure;
        std::string data = js.dump();
        sendMsg(fd, FriendAdd, data);
        return;
    }
    if(r.Sismember(mid+"000",uid )){
        r.Hmset(mid + "f",uid, uname);
        r.Hmset(uid + "f",mid, mname);
        {
            Json js;
            js["Msg"] = "已成功添加" + uname +"<" + uid + ">为好友！";
            js["Status"] = Success;
            std::string data = js.dump();
            sendMsg(fd,FriendAdd,data);
            r.Srem(mid+"000", uid);
        }
        Json json;
        json["Uid"] = uid;
        json["Uname"] = uname;
        std::string data2 = json.dump();
        sendMsg(fd, ReFreshFriendList, data2.data());
        if(onlinelist.find(uid) != onlinelist.end()){
            json["Uid"] = mid;
            json["Uname"] = mname;
            data2 = json.dump();
            sendMsg(onlinelist[uid], ReFreshFriendList, data2);
        }
        return;
    }
    if(onlinelist.find(uid) != onlinelist.end()){
        sendMsg(onlinelist[uid], FriendAddMsg, mid);
    }
    Json js;
    js["Msg"] = "已向用户：" + uname +"<" + uid + "> 发送好友请求！";
    js["Status"] = Success;
    std::string data = js.dump();
    sendMsg(fd,FriendAdd,data);
    std::string key = uid+"000";
    r.Sadd(key,mid);
}

void Server::refuseAddFriend(int fd, std::string str){
    sendMsg(fd, PopFriendAddList, str);
    Redis r;
    std::string id = users[fd];
    r.Srem(id + "000", str);
}

void Server::acceptAddFrined(int fd, std::string str){
    sendMsg(fd, PopFriendAddList, str);
    std::string uid = str;
    std::string mid = users[fd];
    std::string uname = getusername(uid);
    std::string mname = getusername(mid);
    Redis r;
    if(!r.Sismember(mid+"000",uid)){
        sendMsg(fd,Refuse,"111");
        return;
    }
    r.Srem(mid+"000",uid);
    r.Hmset(mid + "f", uid, uname);
    r.Hmset(uid + "f", mid, mname);
    if(onlinelist.find(uid) != onlinelist.end()){
        Json js;
        js["Uid"] = mid;
        js["Uname"] = mname;
        std::string data = js.dump();
        sendMsg(onlinelist[uid], ReFreshFriendList, data);
    }
    Json js;
    js["Uid"] = uid;
    js["Uname"] = uname;
    std::string data = js.dump();
    sendMsg(fd, ReFreshFriendList, data);
}

void Server::bannedFriend(int fd, std::string str){
    std::string mid = str.substr(0,9);
    std::string uid = str.substr(9,9);
    Redis r;
    if(r.Sismember(mid+"111",uid)){
        r.Srem(mid+"111", uid);
    }else{
        r.Sadd(mid+"111", uid);
    }

}

std::string Server::getusername(std::string uid){
    Redis r;
    std::string info = r.Hget(UserInfo,uid);
    Json js = Json::parse(info.data());
    std::string uname = js["username"].get<std::string>();
    return uname;
}

std::unordered_map<std::string,std::string> getFl(const std::string &key){
    std::string str = key + "f";
    Redis redis;
    std::unordered_map<std::string,std::string> list = redis.Hmget(str);
    if(list.empty()){
        std::string a = " ";
        return {{a,a}};
    }
    return list;
}
std::unordered_map<std::string,std::string> getGl(const std::string &key){
    std::string str = key + "g";
    Redis redis;

    std::unordered_map<std::string,std::string> list = redis.Hmget(str);
    if(list.empty()){
        std::string a = " ";
        return {{a,a}};
    }
    return list;
}
std::unordered_map<std::string,std::string> getMl(const std::string &key){
    std::string str = key + "m";
    Redis redis;

    std::unordered_map<std::string,std::string> list = redis.Hmget(str);
    if(list.empty()){
        std::string a = " ";
        return {{a,a}};
    }

    return list;
}

