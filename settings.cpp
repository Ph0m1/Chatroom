#include "settings.h"
#include "ui_settings.h"

#include <QMessageBox>

Settings::Settings(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Settings)
{
    ui->setupUi(this);
    this->setWindowTitle("设置");
    this->setWindowIcon(QIcon(":/Header/Header.jpeg"));


    connect(ui->pushButton_2,&QPushButton::clicked,[=](){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::warning(this, "警告","您确定要注销该账户吗？\n(注意，这是一个不可逆的操作!)",QMessageBox::Cancel | QMessageBox::Ok);
        if(reply == QMessageBox::Ok){
            emit deleteAccont();
            this->close();
        }

    });

    connect(ui->pushButton, &QPushButton::clicked,[=](){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::warning(this, "警告","您将要退出Ph0m's Chatroom!", QMessageBox::Cancel | QMessageBox::Ok);
        if(reply == QMessageBox::Ok){
            emit exit();
            this->close();
        }
    });

}

Settings::~Settings()
{
    delete ui;
}
