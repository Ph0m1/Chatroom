#include "log.h"
#include "ui_log.h"
#include "reg.h"
#include "menu.h"
#include "found.h"
#include "menu2.h"
#include <QMessageBox>
log::log(QWidget *parent,QString id,int sfd, int port, std::string ip)
    : QWidget(parent)
    , ui(new Ui::log)
    , fd(sfd)
    , port(port)
    , ip(ip)
{
    ui->setupUi(this);
    // 设置图标
    this->setWindowIcon(QIcon(":/Header/Header.jpeg"));
    // 设置名称
    this->setWindowTitle("Ph0m's chatroom");
    // 设置退出按钮
    if(!id.isEmpty()){
        ui->uid_input->setText(id);
        ui->passwd_input->setFocus();
    }
    connect(ui->quit,&QPushButton::clicked,[=](){
        this->close();
    });
    // 设置登陆按钮
    connect(ui->yes,&QPushButton::clicked,[=](){
        if(ui->uid_input->text().isEmpty()){
            QMessageBox::warning(this,"警告","请填写帐号或邮箱");
            ui->uid_input->focusProxy();
            return ;
        }
        if(ui->passwd_input->text().isEmpty()){
            QMessageBox::warning(this,"警告","请填写密码");
            ui->passwd_input->focusProxy();
            return;
        }
        nlohmann::json js;
        std::string id = ui->uid_input->text().toStdString().c_str();
        std::string passwd = ui->passwd_input->text().toStdString();

        js["id"] = id;
        js["passwd"] = passwd;
        std::string data = js.dump();
        sendMsg(sfd, UserLogin,data);
        std::string remsg;
        MsgType a = recvMsg(sfd,remsg);
        if(a == Success){
            this->close();
            Menu2 *wt = new Menu2(nullptr,sfd,remsg,port,ip);
            wt->show();
        }
        else if (a == Failure){
            ui->passwd_input->clear();
            QMessageBox::warning(this,"警告","帐号或密码错误，请重试!");
        }else if(a == Refuse){
            ui->passwd_input->clear();
            QMessageBox::warning(this,"警告",remsg.c_str());
        }
    });
    // 设置注册按钮
    connect(ui->resButton,&QPushButton::clicked,[=](){
        this->close();
        class reg *widget = new class reg(nullptr,sfd, port, ip);
        widget->show();
    });

    // 设置忘记密码按钮
    connect(ui->foundButton,&QPushButton::clicked,[=](){
        this->close();
        Found *w = new Found(nullptr,sfd, port,ip);
        w->show();
    });
}

log::log(QWidget *parent,int sfd, int port, std::string ip)
    : QWidget(parent)
    , ui(new Ui::log)
    , fd(sfd)
    , port(port)
    , ip(ip)
{
    ui->setupUi(this);
    // 设置图标
    this->setWindowIcon(QIcon(":/Header/Header.jpeg"));
    // 设置名称
    this->setWindowTitle("Ph0m's chatroom");
    // 设置退出按钮
    connect(ui->quit,&QPushButton::clicked,[=](){
        this->close();
    });
    // 设置登陆按钮
    connect(ui->yes,&QPushButton::clicked,[=](){
        try{if(ui->uid_input->text().isEmpty()){

            QMessageBox::warning(this,"警告","请填写帐号或邮箱");
            ui->uid_input->focusProxy();
        }
        if(ui->passwd_input->text().isEmpty()){

            QMessageBox::warning(this,"警告","请填写密码");
            ui->passwd_input->focusProxy();
        }
        nlohmann::json js;
        std::string id = ui->uid_input->text().toStdString().c_str();
        std::string passwd = ui->passwd_input->text().toStdString();

        js["id"] = id;
        js["passwd"] = passwd;
        std::string data = js.dump();
        sendMsg(sfd, UserLogin,data);
        std::string remsg;
        MsgType a = recvMsg(sfd,remsg);
        if(a == Success){
            this->close();
            Menu2 *wt = new Menu2(nullptr, sfd, remsg, port, ip);
            wt->show();
        }
        else if (a == Failure){
            ui->passwd_input->clear();
            QMessageBox::warning(this,"警告","帐号或密码错误，请重试");
        }else if(a == Refuse){
            qDebug() << "dasdad";
            ui->passwd_input->clear();
            QMessageBox::warning(this,"警告",remsg.c_str());
        }
        }catch (const std::exception& e) {
            QMessageBox::critical(this, "错误", QString::fromStdString(e.what()));
        } catch (...) {
            QMessageBox::critical(this, "错误", "发生未知错误");
        }
    });
    // 设置注册按钮
    connect(ui->resButton,&QPushButton::clicked,[=](){
        this->close();
        reg *widget = new reg(nullptr, sfd, port, ip);
        widget->show();
    });

    // 设置忘记密码按钮
    connect(ui->foundButton,&QPushButton::clicked,[=](){
        this->close();
        Found *w = new Found(nullptr, sfd, port, ip);
        w->show();
    });
}

log::~log()
{
    delete ui;
}
