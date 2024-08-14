#include "widget.h"
#include "./ui_widget.h"
#include <QDataStream>
#include <QMessageBox>
#include "mysocket.h"
#include <QDateTime>

Widget::Widget(QWidget *parent, QString uname, QString uid, QString name, QString id,int sfd)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    m_name = name;
    m_id = id;
    u_id = uid;
    qDebug()<<m_id;
    qDebug()<<u_id;
    u_name = uname;
    fd = sfd;
    // 链接发送按钮
    connect(ui->sendButton,&QPushButton::clicked,[=](){
        SendMsg();

        qDebug() << "Sender: "<< m_name ;
    });

    // 链接退出按钮
    connect(ui->quitButton,&QPushButton::clicked,[=](){
        this->close();
    });
    // 链接清除按钮
    connect(ui->clearButton,&QPushButton::clicked,[=](){
        ui->msgTextEdit->clear();
        ui->msgTextEdit->setFocus();
    });

    // 设置字体
    connect(ui->fontComboBox,&QFontComboBox::currentFontChanged,[=](const QFont &font){
        ui->msgTextEdit->setFontFamily(font.toString());
        ui->msgTextEdit->setFocus();
    });

    // 设置字体大小
    void (QComboBox:: * sizebox)(const QString &text) = &QComboBox::currentTextChanged;
    connect(ui->sizeBox,sizebox,[=](const QString &text){
        ui->msgTextEdit->setFontPointSize(text.toDouble());
        ui->msgTextEdit->setFocus();
    });

    // 以下参数均为bool类型
        // 加粗
    connect(ui->strongButton,&QToolButton::clicked,[=](bool checked){
        if(checked){
            ui->msgTextEdit->setFontWeight(QFont::Bold);
        }
        else{
            ui->msgTextEdit->setFontWeight(QFont::Normal);
        }
        ui->msgTextEdit->setFocus();
    });
        // 倾斜
    connect(ui->itllicButton,&QToolButton::clicked,[=](bool checked){
        ui->msgTextEdit->setFontItalic(checked);
        ui->msgTextEdit->setFocus();
    });
        // 下划线
    connect(ui->underlineButton,&QToolButton::clicked,[=](bool checked){
        ui->msgTextEdit->setFontUnderline(checked);
        ui->msgTextEdit->setFocus();
    });

    // 设置文件
    f = new FileMenu;
    connect(ui->fileButton, &QToolButton::clicked,[=](){
        f->show();
    });

        connect(this, SIGNAL(fileinfos(std::string)), f, SLOT(addFilelist(std::string)));

    connect(f, SIGNAL(seletedfilename(std::string)), this, SLOT(recvFileask(std::string)));

    connect(f, SIGNAL(filePath(std::string)), this, SLOT(sentFile(std::string)));
}

Widget::Widget(QWidget *parent, QString uname, QString uid, QString name, QString id,int sfd,
               std::unordered_map<std::string, std::string> lists)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    m_name = name;
    m_id = id;
    u_id = uid;
    qDebug()<<m_id;
    qDebug()<<u_id;
    u_name = uname;
    fd = sfd;
    members = lists;

    for(auto &member:members){
        addmember(member.first.substr(0,9), member.second);
    }
    // 链接发送按钮
    connect(ui->sendButton,&QPushButton::clicked,[=](){
        SendMsg();

        qDebug() << "Sender: "<< m_name ;
    });

    // 链接退出按钮
    connect(ui->quitButton,&QPushButton::clicked,[=](){
        this->close();
    });
    // 链接清除按钮
    connect(ui->clearButton,&QPushButton::clicked,[=](){
        ui->msgTextEdit->clear();
        ui->msgTextEdit->setFocus();
    });

    // 设置字体
    connect(ui->fontComboBox,&QFontComboBox::currentFontChanged,[=](const QFont &font){
        ui->msgTextEdit->setFontFamily(font.toString());
        ui->msgTextEdit->setFocus();
    });

    // 设置字体大小
    void (QComboBox:: * sizebox)(const QString &text) = &QComboBox::currentTextChanged;
    connect(ui->sizeBox,sizebox,[=](const QString &text){
        ui->msgTextEdit->setFontPointSize(text.toDouble());
        ui->msgTextEdit->setFocus();
    });

    // 以下参数均为bool类型
    // 加粗
    connect(ui->strongButton,&QToolButton::clicked,[=](bool checked){
        if(checked){
            ui->msgTextEdit->setFontWeight(QFont::Bold);
        }
        else{
            ui->msgTextEdit->setFontWeight(QFont::Normal);
        }
        ui->msgTextEdit->setFocus();
    });
    // 倾斜
    connect(ui->itllicButton,&QToolButton::clicked, this, [=](bool checked){
        ui->msgTextEdit->setFontItalic(checked);
        ui->msgTextEdit->setFocus();
    });
    // 下划线
    connect(ui->underlineButton,&QToolButton::clicked, this, [=](bool checked){
        ui->msgTextEdit->setFontUnderline(checked);
        ui->msgTextEdit->setFocus();
    });
    // 设置文件
    f = new FileMenu;
    connect(ui->fileButton, &QToolButton::clicked, this, [=](){
        f->show();
    });

    connect(this, SIGNAL(fileinfos(std::string)), f, SLOT(addFilelist(std::string)));
    connect(f, SIGNAL(seletedfilename(std::string)), this, SLOT(recvFileask(std::string)));

    connect(f, SIGNAL(filePath(std::string)), this, SLOT(sentFile(std::string)));
}



void Widget::fileinfo(std::string fileinfo){
    Json js = Json::parse(fileinfo.data());
    std::string filename = js["Filename"].get<std::string>();
    std::string sender;
    if(js["Flag"] == 1)
        sender = js["From"].get<std::string>();
    else
        sender = js["To"].get<std::string>();
    if(sender != u_id.toStdString()){
        return;
    }
    emit readmsg(sender);
    ui->MsgBrowser->setTextColor(Qt::gray);
    if(js["Flag"] == 1)
        ui->MsgBrowser->append(QString::fromStdString("对方向你发送了一个文件: " + filename +"，请点击文件菜单接收"));
    else
        ui->MsgBrowser->append(QString::fromStdString(members[js["From"]] + " 发送了一个文件：" + filename + " ,请点击文件菜单接收"));
    emit fileinfos(filename);
}

void Widget::recvFileask(std::string filename){
    std::string fileinfo = u_id.toStdString() + m_id.toStdString() + filename;
    emit recvf(fileinfo);
    // sendMsg(fd, AcceptFiles, fileinfo);
}

void Widget::sentFile(std::string filepath){

    if(filepath == " "){
        QMessageBox::information(this, "提示", "请选择正确的文件路径");
        return;
    }
    emit sendf(filepath, u_id.toStdString());
    // sendFile(fd, filepath, u_id.toStdString());
}


void Widget::SendMsg(){
    if(ui->msgTextEdit->toPlainText().isEmpty()){
        QMessageBox::warning(this,"警告","输入不可为空");
        return;
    }
    QString mm = getMsg();

    std::string msg = m_id.toStdString() + u_id.toStdString() + mm.toStdString();

    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    std::string t = time.toStdString();
    Json stream = {{"Time",t},{"Msg",msg},{"Status",Unread}};
    // stream["Time"] = t;
    // stream["Msg"] = msg;
    std::string data = stream.dump();
    getData(data);
    qDebug() << fd;
    sendMsg(fd, Msg, data);
}

QString Widget::getName(){
    return this->m_name;
}

QString Widget::getMsg(){
    QString msg = ui->msgTextEdit->toHtml();// 返回输入框的内容
    ui->msgTextEdit->clear();
    ui->msgTextEdit->setFocus();
    return msg;
}

void Widget::getData(std::string data){
    Json js = Json::parse(data.data());
    std::string msg = js["Msg"].get<std::string>();
    std::string time = js["Time"].get<std::string>();
    std::string sender = msg.substr(0,9);
    std::string recver = msg.substr(9,9);
    std::string uid = u_id.toStdString();
    std::string mid = m_id.toStdString();
    // 0表示自己发出的，1表示别人发给自己的，2表示通知
    if(uid == sender){
        emit readmsg(uid);
        // std::cout<<uid;
        if(recver == mid){
            prints(time, msg.substr(18), 1);
        }
        else if(recver == "000000000")
            prints(time, msg.substr(18), 2);
        else{
            prints(time, msg.substr(9),3);
        }
    }
    else if(uid == recver){
        if(mid == sender){
            prints(time, msg.substr(18), 0);
        }else{
            prints(time,msg.substr(18),2);
        }
    }

}
void Widget::inithistory(std::vector<std::string> ml){
    std::unordered_map<std::string ,std::string> mp;
    for(auto & t: ml)
    {
        Json js = Json::parse(t.data());
        mp[js["Time"].get<std::string>()] = t.data();
    }
    std::vector<std::pair<std::string, std::string>> mv(mp.begin(),mp.end());
    std::sort(mv.begin(),mv.end(), [](const auto& a, const auto& b) {
        return a.first < b.first;
    });
    for(auto &t : mv){
        Json js = Json::parse(t.second.data());
        std::string msg = js["Msg"].get<std::string>();
        std::string time = js["Time"].get<std::string>();
        std::string sender = msg.substr(0,9);
        std::string recver = msg.substr(9,9);
        std::string uid = u_id.toStdString();
        std::string mid = m_id.toStdString();
        //flag : 0表示自己发出的，1表示别人发给自己的, 2表示通知消息，3表示群聊消息
        if(uid == sender){
            if(mid == recver)
                prints(time, msg.substr(18), 1);
            else if(recver == "000000000")
                prints(time, msg.substr(18), 2);
            else
                prints(time, msg.substr(9), 3);
        }
        else if(uid == recver){
            prints(time, msg.substr(18), 0);
        }
    }
}

void Widget::addmember(std::string id, std::string name){
    members[id] = name;
    adduser(id, name);
}
// 向成员列表中加入某人
void Widget::adduser(std::string id, std::string name){
    QString info = static_cast<QString>((name + "(" + id + ")").c_str());
    bool isEmpty = ui->userTable->findItems(info, Qt::MatchExactly).isEmpty();
    if(isEmpty){
        QTableWidgetItem *user = new QTableWidgetItem(info);
        ui->userTable->insertRow(0);
        ui->userTable->setItem(0,0,user);
    }
}
// 从成员列表中删除某人
void Widget::deleteuser(std::string id, std::string name){
    QString info = static_cast<QString>((name + "(" + id + ")").c_str());
    bool isEmpty = ui->userTable->findItems(info, Qt::MatchExactly).isEmpty();
    if(!isEmpty){
        QTableWidgetItem *user = new QTableWidgetItem(info);
        int row = ui->userTable->findItems(info, Qt::MatchExactly).first()->row();
        ui->userTable->removeRow(row);
    }
}

// 打印通知
void Widget::printinfo(std::string msg){
    ui->MsgBrowser->setTextColor(Qt::gray);
    ui->MsgBrowser->append(msg.c_str());
}
// 打印消息
void Widget::prints(std::string time, std::string msg, int flag){
    ui->MsgBrowser->setTextColor(Qt::blue);
    ui->MsgBrowser->setCurrentFont(QFont("Times New Roman",10));
    if(flag == 1){
        ui->MsgBrowser->append("["+u_name+"] " + static_cast<QString>(time.c_str()));
    }
    else if(flag == 0){
        ui->MsgBrowser->setTextColor(Qt::green);
        ui->MsgBrowser->append("["+m_name+"] " + static_cast<QString>(time.c_str()));
    }
    else if(flag == 2){
        qDebug() << "NIDAINDA";
        printinfo(msg);
        return;
    }else if(flag == 3){
        ui->MsgBrowser->setTextColor(Qt::blue);
        ui->MsgBrowser->append("[" + static_cast<QString>(members[msg.substr(0,9)].c_str())+"] "
                               + static_cast<QString>(time.c_str()));
        ui->MsgBrowser->append(msg.substr(9).c_str());
        return;
    }
    ui->MsgBrowser->append(msg.c_str());
}
void Widget::closeEvent(QCloseEvent *){
    emit this->closeWidget();
    // sendMsg(UserLeft);
}

Widget::~Widget()
{
    delete ui;
}
