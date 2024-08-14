#include "setnewpasswd.h"
#include "ui_setnewpasswd.h"
#include "log.h"
#include <QMessageBox>
SetNewPasswd::SetNewPasswd(QWidget *parent,int sfd, int port, std::string ip)
    : QWidget(parent)
    , ui(new Ui::SetNewPasswd)
    , fd(sfd)
    , port(port)
    , ip(ip)
{
    ui->setupUi(this);
    // 设置图标
    this->setWindowIcon(QIcon(":/Header/Header.jpeg"));
    // 设置名称
    this->setWindowTitle("请确认你的密码");
    // 确认修改密码
    connect(ui->yesBtn,&QPushButton::clicked,[=](){
        std::string str = ui->newpasswdEdit->text().toStdString().c_str();
        sendMsg(fd,ResetPasswd,str);
        std::string buf;
        MsgType status = recvMsg(fd,buf);
        if(status != Success){
            QMessageBox::warning(this,"警告","修改密码失败！");
            return;
        }
        QMessageBox::information(this,"成功！","点击ok返回登陆页面");
        this->close();
        class log *w = new class log(nullptr, sfd, port, ip);
        w->show();
    });

    connect(ui->quitBtn,&QPushButton::clicked,[=](){
        this->close();
        class log *w = new class log(nullptr, sfd, port, ip);
        w->show();
    });
}

SetNewPasswd::~SetNewPasswd()
{
    delete ui;
}
