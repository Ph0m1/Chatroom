#include "client.h"
#include "log.h"

Client::Client(int argc,char *argv[]) {
    this->m_sfd = socket(AF_INET,SOCK_STREAM,0);
    if(this->m_sfd<0){
        std::cout<<"Socket error!"<<std::endl;
        exit(-1);
    }
    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    ip = "127.0.0.1";
    port = 9098;

    if(argc >= 2){
        port = std::stoi(argv[1]);
    }
    if(argc >= 3){
        port = std::stoi(argv[2]);
    }
    if(argc >=3){
        ip = argv[1];
    }
    addr.sin_port = htons(port);
    if(inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0){
        std::cout << "Invalid address/ Address not supported" << std::endl;
        exit(-1);
    }
    if(connect(this->m_sfd,(struct sockaddr*)&addr,sizeof(addr)) < 0){ //链接服务端
        std::cout<<"connect error"<<std::endl;
        exit(-1);
    }
    std::cout <<"client started"<<std::endl;

    class log *ui = new class log(nullptr,this->m_sfd, port, ip);
    ui->show();
}

Client::~Client(){
    sendMsg(this->m_sfd,Disconnent,"0");
}
