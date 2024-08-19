#include "mysocket.h"

void sendMsg(int fd,MsgType type){
    send(fd,&type,sizeof(type),0);
}

MsgType recvMsg(int sfd){
    MsgType a;
    int valread = read(sfd,&a,sizeof(a));
    if (valread >= 0){
        return a;
    }
    return Failure;
}

void sendFile(int sfd,const std::string &filePath, std::string uid, std::string mid){
    int file_fd = open(filePath.c_str(), O_RDONLY);
    if(file_fd < 0){
        std::cerr <<"Failed to open file: " << filePath << std::endl;
        return;
    }

    size_t filenamepos = filePath.find_last_of("/\\");
    std::string filename = filePath.substr(filenamepos + 1);
    struct stat file_stat;
    if(fstat(file_fd, &file_stat) < 0){
        std::cerr << "Failed to get file stats" <<std::endl;
        close(file_fd);
        return;
    }
    Json js;
    js["Filename"] = filename;
    js["Size"] = file_stat.st_size;
    js["To"] = uid;
    js["From"] = mid;
    sendMsg(sfd, File, js.dump());

    off_t offset = 0;
    ssize_t sent_bytes = 0;
    std::cout << "[SENDING FILE] " << filename << std::endl;
    while(offset < file_stat.st_size){
        sent_bytes = sendfile(sfd, file_fd, &offset, file_stat.st_size - offset);
        std::cout << "\33[2K\r" << "File is sending: " << (offset * 100)/ file_stat.st_size << "%" << std::flush;
        if(sent_bytes < 0){
            std::cerr << "Failed to send file" <<std::endl;
            close(file_fd);
            return;
        }
    }
    std::cout << std::endl;
    std::cout << "File sent successfully" << std::endl;
    close(file_fd);
}

void recvFile(int fd, size_t filesize, const std::string &filename, const std::string &pathdir){
    std::filesystem::path current_path = std::filesystem::current_path();
    if(!std::filesystem::exists(current_path / pathdir)){
        std::filesystem::create_directories(current_path / pathdir);
    }
    int file_fd = open((current_path / pathdir / filename).c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(file_fd < 0){
        std::cerr << "Failed to open file: " << current_path / pathdir / filename << std::endl;
        return;
    }

    size_t received_size = 0;
    ssize_t received_bytes;
    char buffer[5242880];
    std::cout << "[RECVING FILE] " << filename << std::endl;
    while(received_size < filesize){
        received_bytes = recv(fd, buffer, sizeof(buffer) <= filesize - received_size? sizeof(buffer) : filesize - received_size, 0);
        if(received_bytes < 0){
            std::cerr << "Failed to receive file data" << std::endl;
            close(file_fd);
            return;
        }

        if(received_bytes == 0){
            // 连接关闭，提前结束
            break;
        }
        // 写入文件
        if(write(file_fd, buffer, received_bytes) != received_bytes){
            std::cerr << "Failed to write to file" << std::endl;
            close(file_fd);
            return;
        }

        received_size += received_bytes;
        std::cout << "\33[2K\r" <<"File receiving: " << (received_size * 100) / filesize << "%" << std::flush;

    }
    std::cout << std::endl;
        if(received_size == filesize){
            std::cout << "File received successfully" << std::endl;
        }else{
            std::cerr << "File transfer incomplete" << std::endl;
        }
        close(file_fd);
}

void sendMsg(int sfd,MsgType type,const std::string &msg){


    std::cout << "sendMsg started" << std::endl;

    if (sfd == -1 || msg.empty()) {
        std::cout << "Invalid socket or empty message" << std::endl;
        return;
    }
    std::cout << "Preparing JSON" << std::endl;
    nlohmann::json js;
    js["MsgType"] = type;
    js["Msg"] = msg;
    std::string data = js.dump();

    std::cout << "JSON prepared: " << data << std::endl;
    int len = htonl(data.size());
    std::string buf(4 + data.size(), '\0');

    memcpy(&buf[0], &len, 4);
    memcpy(&buf[4], data.data(), data.size());

    std::cout << "Buffer prepared, sending message" << std::endl;
    int ret = writen(sfd, buf, buf.size());
    if (ret < 0) {
        std::cerr << "Writing error" << std::endl;
        close(sfd);
    }
    std::cout << "sendMsg finished" << std::endl;
}


int writen(int fd,std::string buf, int len){
    const char *p = buf.c_str();
    int count = len;
    while(count > 0){
        int ret = send(fd, p ,count,0);
        if(ret <= -1){
            if(errno == EINTR && ret == -1){
                continue;
            }
            else
            {
                std::cerr << "Error sending data: "<<strerror(errno) << std::endl;
                return -1;
            }
        }
        else if(ret == 0){
            continue;
        }
        p +=ret;
        count -= ret;
    }
    return len;
}


MsgType recvMsg(int sfd,std::string &str){

    int len = 0;
    read(sfd,&len,4);
    len = ntohl(len);
    std::vector<char> buf(len+1);
    int ret = readen(sfd,buf.data(), len);
    if(ret == 0){
        return Disconnent;
    }
    else if( len != ret){
        return Failure;
    }
    buf[len] = '\0';
    std::cout << "[BUFFERDATAS]"<< buf.data() << std::endl;
    nlohmann::json js = nlohmann::json::parse(buf.data());

    str.append(js["Msg"].get<std::string>());
    if (ret > 0)
        return js["MsgType"].get<MsgType>();

    return Failure;
}




int readen(int fd, char* buf, int len){
    int count = len;
    char *current = buf;
    while(count > 0 ){
        int ret = recv(fd,current,count,0);
        if(ret <= -1){
            if(errno == EINTR && ret == -1){
                continue;
            }else{
                std::cerr<< "Error receiving data: " << strerror(errno) << std::endl;
                close(fd);
                return -1;
            }
        }
        else if(ret == 0){
            break;
        }

        current += ret;
        count -= ret ;
    }
    return len-count;
}


