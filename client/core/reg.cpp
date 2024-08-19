#include "reg.h"
#include "ui_reg.h"
#include "log.h"
#include <QMessageBox>
reg::reg(QWidget *parent,int sfd, int port, std::string ip)
    : QWidget(parent)
    , ui(new Ui::reg)
    , port(port)
    , ip(ip)
{
    ui->setupUi(this);
    // 设置图标
    this->setWindowIcon(QIcon(":/Header/Header.jpeg"));
    // 设置名称
    this->setWindowTitle("注册账户");

    // 设置注册按钮
    connect(ui->regBtn,&QPushButton::clicked,[=](){
        if(ui->usrEdit->text().isEmpty() || ui->mailEdit->text().isEmpty() || ui->passwdEdit->text().isEmpty()){
            QMessageBox::information(this,"提示","请输入完整信息");
            return;
        }
        QString qs = nullptr ;
        qs = ui->usrEdit->text();
        std::string username = qs.toStdString();
        qs = ui->mailEdit->text();
        std::string mail = qs.toStdString();
        qs = ui->passwdEdit->text();
        std::string passwd = qs.toStdString();
        nlohmann::json js;
        {
            js["mail"] = mail.c_str();
            js["username"] = username.c_str();
            js["passwd"] = passwd.c_str();
        }
        std::string msg = js.dump();

        sendMsg(sfd,UserAccount,msg);
        std::string remsg;
        MsgType t = recvMsg(sfd,remsg);
        if(t == Failure){
            QMessageBox::warning(this,"错误","注册失败，请重试");
            return;
        }
        else if(t == Refuse){
            QMessageBox::warning(this,"警告",remsg.c_str());
        }
        else{
        QMessageBox::information(this,"注册成功！您的帐号为：", remsg.c_str());
        this->close();
        QString id = static_cast<QString>(remsg.c_str());
        class log *widget = new class log(nullptr,id,sfd,port,ip);
        widget->show();
        }
    });
    // 设置返回按钮
    connect(ui->returnBtn,&QPushButton::clicked,[=](){
        this->close();
        class log *widget = new class log(nullptr,sfd,port,ip);
        widget->show();
        // QMessageBox::warning(this,QString("ausjidhasiojda"),"ni");
    });
}

reg::~reg()
{
    delete ui;
}
