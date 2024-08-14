#include "menu2.h"
#include "ui_menu2.h"
#include "mysocket.h"
#include "widget.h"
#include "msgtype.h"
#include "informations.h"
#include "settings.h"
#include "badgetoolbutton.h"
#include <QPainter>
#include <QToolButton>
#include <QPainterPath>
using json = nlohmann::json;

Menu2::Menu2(QWidget *parent, int sfd, const std::string& data, int port, std::string ip)
    : QWidget(parent)
    , fd(sfd)
    , ui(new Ui::Menu2)
    , threadPool(new ThreadPool(4))
    , port(port)
    , ip(ip)
{
    layout = new QVBoxLayout();
    // 注册常用类型
    qRegisterMetaType<std::string>("std::string");
    qRegisterNormalizedMetaType<std::vector<std::string>>("std::vector<std::string>");
    //解析data数据包
    std::cout<<data<<std::endl;
    json datajs = json::parse(data.data());

    json infojs = datajs["Info"].get<json>();
    //fl id:name
    //ml time(15位):<发送方id(9位)><接受方id(9位)><内容>
    friendaddlist = datajs["FriendAdd"].get<std::vector<std::string>>();
    std::unordered_map<std::string,std::string> fl = datajs["FriendList"].get<std::unordered_map<std::string,std::string>>();
    qDebug()<<"12";
    std::unordered_map<std::string,std::string> gl = datajs["GroupList"].get<std::unordered_map<std::string,std::string>>();
    std::unordered_map<std::string,std::string> ml = datajs["MsgList"].get<std::unordered_map<std::string,std::string>>();
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> gmembers
        = datajs["GroupMembers"].get<std::unordered_map<std::string, std::unordered_map<std::string, std::string>>>();
    m_name = infojs["username"].get<std::string>();
    qDebug() << m_name.c_str();
    std::string id;

    id = datajs["Uid"].get<std::string>();
    qDebug() << id.c_str();
    this->m_id = id;
    ui->setupUi(this);

    // 设置图标
    this->setWindowIcon(QIcon("/Header/Header.jpeg"));

    // 设置名称
    this->setWindowTitle("Rooms");

    // 加载图像
    QPixmap pixmap(":/Header/Header.jpeg");

    if (pixmap.isNull()) {
        qDebug() << "Failed to load pixmap.";
        return;
    }
    // 裁剪成圆形
    QPixmap circularPixmap(pixmap.size());
    circularPixmap.fill(Qt::transparent);

    {
        QPainter painter(&circularPixmap);
        if (!painter.isActive()) {
            qDebug() << "Failed to create QPainter.";
            return;
        }
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);

        QPainterPath path1;
        path1.addEllipse(0, 0, pixmap.width(), pixmap.height());
        painter.setClipPath(path1);
        painter.drawPixmap(0, 0, pixmap);
        painter.end();
    }
    // 设置按钮图标
    ui->infoBtn->setIcon(QIcon(circularPixmap));

    // 设置图标大小
    circularPixmap = circularPixmap.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->infoBtn->setIconSize(circularPixmap.size());

    // 设置按钮样式
    ui->infoBtn->setAutoRaise(true);
    ui->infoBtn->setStyleSheet("background-color: none; border: none; border-radius: 25px;");

    // 设置聊天界面按钮
    QPixmap cpixmap = circularPixmap;
    cpixmap = cpixmap.scaled(32,32,Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->msgBtn->setAutoRaise(true);
    ui->msgBtn->setIconSize(cpixmap.size());
    ui->friendBtn->setAutoRaise(true);
    ui->friendBtn->setIconSize(cpixmap.size());
    ui->settingBtn->setAutoRaise(true);
    ui->settingBtn->setIconSize(cpixmap.size());
    Menu2::btnIsChecked[0] = false;
    Menu2::btnIsChecked[1] = true;
    Menu2::setFbtn(fl,1, gmembers);
    Menu2::setFbtn(gl,2, gmembers);
    Menu2::setMbtn(ml);
    friendaddbtn = new BadgeToolButton;
    groupinfobtn = new BadgeToolButton;

    friendaddbtn->setText("新朋友    ");
    groupinfobtn->setText("群通知    ");
    friendaddbtn->setUnreadCount(friendaddlist.size());
    groupinfobtn->setUnreadCount(groupinfolist.size());
    w = new informations(nullptr, fd, friendaddlist, 0);
    w2 = new informations(nullptr, fd, groupinfolist, 1);
    connect(friendaddbtn, &BadgeToolButton::clicked, this, [=](){
        w->show();
        friendaddbtn->setUnreadCount(0);
    });
    connect(groupinfobtn, &BadgeToolButton::clicked, this, [=](){
        w2->show();
        groupinfobtn->setUnreadCount(0);
    });
    ui->framelayout->addWidget(friendaddbtn);
    ui->framelayout->addWidget(groupinfobtn);

    // Menu2::setFbtn(ml);
    connect(ui->friendBtn,&QToolButton::clicked, this, [=](){
        if(Menu2::btnIsChecked[0] == true || Menu2::btnIsChecked[1] == false){
            return;
        }
        Menu2::btnIsChecked[1] = false;
        Menu2::btnIsChecked[0] = true;
        ui->msgBtn->setChecked(false);
        // ui->friendBtn->setChecked(false);
        ui->friendBtn->setChecked(true);

    });
    connect(ui->msgBtn,&QToolButton::clicked, this, [=](){
        if(Menu2::btnIsChecked[1] == true || Menu2::btnIsChecked[0] == false){
            return;
        }
        Menu2::btnIsChecked[1] = true;
        Menu2::btnIsChecked[0] = false;
        ui->friendBtn->setChecked(false);
    });
    connect(ui->settingBtn,&QToolButton::clicked, this, [=](){
        setting->show();
    });
    connect(ui->addBtn, &QToolButton::clicked, this, [=](){
        std::string line = ui->searchEdit->text().toStdString();
        if(line.empty()){
            QMessageBox::warning(this,"错误！","输入不能为空!");
            return;
        }
        sendMsg(fd,FriendAdd,line);

    });
    // 绑定功能按键
    //

    connect(ui->inviteBtn, &QToolButton::clicked, this, &Menu2::showCreateGroupDialog);
    connect(ui->deletefriendBtn, &QToolButton::clicked, this, &Menu2::showDeleteFriendDialog);

    // 绑定信号和槽
    connect(setting, SIGNAL(deleteAccont()), this, SLOT(deleteAccont()));
    connect(this, SIGNAL(addRowList(std::string)), w, SLOT(addfriendRow(std::string)));
    connect(this, SIGNAL(addgroupRow(std::string)), w2, SLOT(addgroupRow(std::string)));
    connect(this, SIGNAL(friendsg(std::string)), this,  SLOT(friendAdd(std::string)));
    connect(this, SIGNAL(refreshFriendList(std::string)),
            this, SLOT(updateFriendList(std::string)));
    // connect(this, SIGNAL(receviedfile(std::string)), this, SLOT(recvfile(std::string)));
    connect(this, SIGNAL(friendaddmsg(std::string)),
            this, SLOT(updatefriendaddbtn(std::string)));
    connect(setting, SIGNAL(exit()), this, SLOT(exit()));
    // connect(this, SIGNAL(newMsg(std::string)), this, SLOT(updateList(std::string)));
    // 启动线程池
    threadPool->init();

    // 启动一个线程来接收服务器消息
    threadPool->submit([this] { readFromServer(this->fd); });
    // readFromServer(fd);
}



void Menu2::sendFile(const std::string &filepath, std::string uid){
    FileSocket *sock = new FileSocket (this->ip, this->port);
    threadPool->submit([=](){
        FileSocket *sock = new FileSocket (this->ip, this->port);
        // 修改sendFile打包的json
        ::sendFile(sock->getfilefd(), filepath, uid, m_id);
        sendMsg(sock->getfilefd(), Disconnent, "[Finshed work]");
        delete sock;
    });
}


void Menu2::recvfile(std::string fileinfo){
    threadPool->submit([=](){
        FileSocket *sock = new FileSocket(this->ip, this->port);
        sendMsg(sock->getfilefd(), AcceptFiles, fileinfo);
        std::string newinfo;
        MsgType status = recvMsg(sock->getfilefd(), newinfo);
        qDebug() << newinfo.c_str();
        if(status != File){
            delete sock;
            return;
        }
        Json js = Json::parse(newinfo.data());
        size_t filesize = js["Size"].get<size_t>();
        std::string filename = js["Filename"].get<std::string>();

        recvFile(sock->getfilefd(), filesize, filename, "received_files");
        sendMsg(sock->getfilefd(), Disconnent, "[Finshed work]");
        delete sock;
    });
    // recvFile(fd, filesize, filename, "received_files");
    // resume();
}
void Menu2::readFromServer(int fd){

    while(true){
        // std::unique_lock<std::mutex> lock(pauseMutex);
        // pauseCondition.wait(lock, [this] { return !pauseThread; });
        try
        {
            std::string buffer;
            MsgType type = recvMsg(fd,buffer);
            switch (type){
            case Msg:
                emit sendData(buffer);
                break;
            case ReFreshFriendList:
                emit refreshFriendList(buffer);
                break;
            case FriendAdd:
                emit friendsg(buffer);
                break;
            case FriendAddMsg:
                emit friendaddmsg(buffer);
                break;
            case FileInfo:
                emit fileinfos(buffer);
                break;
            case File:
                // pause();
                emit receivedfile(buffer);
                break;
            case PopFriendAddList:
                auto t = std::remove(friendaddlist.begin(), friendaddlist.end(), buffer);
                friendaddlist.erase(t);
                break;

            }
        }
        catch(...){

        }
    }
}

void Menu2::pause(){
    std::lock_guard<std::mutex> lock(pauseMutex);
    pauseThread = true;
}

void Menu2::resume(){
    {
    std::lock_guard<std::mutex> lock(pauseMutex);
    pauseThread = false;
    }
    pauseCondition.notify_all();
}

void Menu2::showCreateGroupDialog(){
    CreateGroupDialog dialog(this, friendlist); // 传递好友列表
    if(dialog.exec() == QDialog::Accepted){
        std::vector<std::string> selectedFriends = dialog.getSelectedFriends();
        std::string gname = dialog.getGroupName();
        // 处理选择的好友并创建群聊
        if(gname == " ") return;
        if(!selectedFriends.empty()){
            // 将好友id传递到创建群聊的函数
            createGroup(selectedFriends, gname);
        }
        else {
            QMessageBox::warning(this, "警告", "请至少选择一个好友来创建群聊!");
        }
    }
}

void Menu2::showDeleteFriendDialog(){
    CreateFriendDialog dialog(this, friendlist);
    if(dialog.exec() == QDialog::Accepted){
        std::vector<std::string> selectedFriends = dialog.getSelectedFriends();

        if(!selectedFriends.empty()){
            // 传递至删除好友的函数
            deletefriends(selectedFriends);
        }
    }
}

void Menu2::createGroup(const std::vector<std::string> &friends, const std::string &name){
    Json js;
    js["Members"] = friends;
    js["Gname"] = name;
    js["Owner"] = m_id;
    std::string data = js.dump();
    sendMsg(fd, GroupCreat, data);
}

void Menu2::deletefriends(const std::vector<std::string> &friends){
    Json js;
    js["Info"] = friends;
    std::string data = js.dump();
    sendMsg(fd, FriendDelete, data);
    // removefbtn();
}

void Menu2::updatefriendaddbtn(std::string id){
    friendaddbtn->addUnreadCount(1);
    auto t = std::find(friendaddlist.begin(), friendaddlist.end(), id);
    if(t == friendaddlist.end()){
        friendaddlist.push_back(id);
        emit addRowList(id);
    }
}


void Menu2::updateList(std::string id){

    for(auto &t : lists){
        if(t.first == id){
            t.second->addUnreadCount(1);
        }
    }
    // QVBoxLayout *layout = new QVBoxLayout;
    // while (QLayoutItem* item = ui->listLayout->takeAt(0)) {
    //     if (QWidget* widget = item->widget()) {
    //         widget->deleteLater();
    //     }
    //     delete item;
    // }
    // for(auto &t : lists){
    //     if(t.first == id){
    //         t.second->addUnreadCount(1);
    //     }
    //     layout->addWidget(t.second);
    // }
    // ui->listLayout->addLayout(layout);
}




void Menu2::updateFriendList(std::string msg){
    qDebug() << msg.c_str();
    resetFbtn(msg);
}

void Menu2::friendAdd(std::string msg){
    Json js = Json::parse(msg.data());
    MsgType status = js["Status"].get<MsgType>();
    std::string res = js["Msg"].get<std::string>();
    qDebug() << res.c_str();
    if(status == Failure){
        QMessageBox::warning(this,"错误！",res.c_str());
        return;
    }
    if(status == Success){
        QMessageBox::information(this, "提示", res.c_str());
        return;
    }
    QMessageBox::information(this, "提示", "已发送好友请求！");
}

void Menu2::setMbtn(std::unordered_map<std::string,std::string> list){
    // QVBoxLayout *layout = new QVBoxLayout();
    std::vector<std::string> mlist;
    for(auto &t : list){
        if (t.first == " "){
            break;
        }
        Json js = Json::parse(t.second.c_str());
        std::string msgInfo = js["Msg"].get<std::string>();
        std::string sender = msgInfo.substr(0,9);
        std::string recver = msgInfo.substr(9,9);
        std::string msg = msgInfo.substr(18);
        mlist.push_back(t.second);
        // MsgType status = js["Status"].get<MsgType>();
        // std::string time = js["Time"].get<std::string>();
        // // 获取flag 0,1为消息，2为通知
        // int flag = 1;
        // if(sender == this->m_id){
        //     flag = 0;
        // }
        // if(sender != this->m_id && recver != this->m_id){
        //     flag = 2;
        // }
        // std::string key = sender == this->m_id ? recver : sender;
        // msglist.push_front(lists[recver]);

        // auto it = std::find(vector.begin(),vector.end(),lists[recver]);
        // if(it == vector.end()){
        //     return;
        // }
        // int index = std::distance(vector.begin(), it); // 拿到下标
        // qDebug() << "New msg form index: " << index << (*it)->text();

        // Widget *currentWidget = qStack->widget(index);
        // printmsg(key, msg, flag, currentWidget);
    }
    std::reverse(mlist.begin(),mlist.end());
    emit sendlist(mlist);
}

void Menu2::resetFbtn(const std::string& str){
    qDebug() << str.c_str();

    Json js = Json::parse(str.data());
    std::string uid = js["Uid"].get<std::string>();
    std::string uname = js["Uname"].get<std::string>();
    BadgeToolButton* btn = new BadgeToolButton(this);
    friendlist[uid] = uname;
    // QToolButton *btn = new QToolButton(this);
    // 加载图标
    btn -> setIcon(QPixmap(":/Header/Header.jpeg"));
    // 设置图片大小
    QPixmap h(":/Header/Header.jpeg") ;
    h = h.scaled(48,48,Qt::KeepAspectRatio, Qt::SmoothTransformation);

    btn->setIconSize(h.size());
    // 设置按钮样式 透明
    btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    btn->setFixedHeight(50);
    btn->setMinimumWidth(60);
    btn->setAutoRaise(true);
    btn->setCheckable(true);
    // 设置网名
    btn->setText(QString(static_cast<QString>(uname.c_str()) + "<" + static_cast<QString>(uid.c_str())) + ">");

    btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);// 设置按钮样式为文字紧贴图片
    // btn->setAutoRepeat(true)；
    ui->listLayout->addWidget(btn);
    this->vector.push_back(btn);
    FriendIsShow.push_back(false);
    Widget *w = new Widget(nullptr, uname.data(), uid.data(), m_name.data(), m_id.data(), fd);
    qStack->addWidget(w);
    lists.insert(std::pair<std::string, BadgeToolButton*>(uid, btn));
    connect(this, SIGNAL(fileinfos(std::string)), w, SLOT(fileinfo(std::string)));
    connect(this, SIGNAL(sendlist(std::vector<std::string>)), w, SLOT(inithistory(std::vector<std::string>)));
    connect(this,SIGNAL(sendData(std::string)),w,SLOT(getData(std::string)));
    connect(w, SIGNAL(readmsg(std::string)), this, SLOT(updateList(std::string)));
    connect(w, SIGNAL(recvf(std::string)), this, SLOT(recvfile(std::string)));

    connect(btn, &QToolButton::clicked,[this, i = vector.count() - 1](){
        vector[i]->setUnreadCount(0);
        FriendIsShow[i] = true;
        qStack->setCurrentIndex(i); // 切换对话框
        ui->label->setText(this->vector[i]->text());
        qDebug() << "Button clicked:" << i << vector[i]->text();
    });
}

void Menu2::setFbtn(std::unordered_map<std::string,std::string> list, int flag,
                    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> gmember){

    // QVector<QToolButton*> vector;

    for(auto &t : list){
        if (t.first == " "){
            break;
        }
        BadgeToolButton *btn = new BadgeToolButton(this);
        // 加载图标
        btn -> setIcon(QPixmap(":/Header/Header.jpeg"));
        // 设置图片大小
        QPixmap h(":/Header/Header.jpeg") ;
        h = h.scaled(48,48,Qt::KeepAspectRatio, Qt::SmoothTransformation);

        btn->setIconSize(h.size());
        // 设置按钮样式 透明
        btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        btn->setFixedHeight(50);
        btn->setMinimumWidth(60);
        btn->setAutoRaise(true);
        btn->setCheckable(true);
        // 设置网名
        btn->setText(QString(static_cast<QString>(t.second.c_str()) + "<" + static_cast<QString>(t.first.c_str())) + ">");

        btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);// 设置按钮样式为文字紧贴图片
        btn->setAutoRepeat(true);
        layout->addWidget(btn);// 将按钮添加到布局
        this->vector.push_back(btn);// 将按钮添加到容器
        FriendIsShow.push_back(false);
        // 添加聊天页面
        Widget *w;
        if(flag == 1)
        {
            friendlist[t.first] = t.second;
            w = new Widget(nullptr, t.second.data(), t.first.data(), m_name.data(), m_id.data(), fd);
        }else if(flag == 2)
        {
            grouplist[t.first] = t.second;
            w = new Widget(nullptr, t.second.data(), t.first.data(), m_name.data(), m_id.data(), fd, gmember[t.first]);
        }
        connect(this, SIGNAL(fileinfos(std::string)), w, SLOT(fileinfo(std::string)));
        connect(this, SIGNAL(sendlist(std::vector<std::string>)), w, SLOT(inithistory(std::vector<std::string>)));
        connect(this,SIGNAL(sendData(std::string)),w,SLOT(getData(std::string)));
        connect(w, SIGNAL(readmsg(std::string)), this, SLOT(updateList(std::string)));
        connect(w, SIGNAL(sendf(std::string,std::string)), this, SLOT(sendFile(std::string,std::string)));
        connect(w, SIGNAL(recvf(std::string)), this, SLOT(recvfile(std::string)));
        qStack->addWidget(w);
        ui->msgLayout->addWidget(qStack,0);
        lists.insert(std::pair<std::string, BadgeToolButton*>(t.first, btn));

    }
    // listStack->addWidget(layout->widget());
    // listStack->addItem(layout);
    ui->listLayout->addLayout(layout);
    // ui->listWidget->setLayout(layout);


    for(int i = 0; i < vector.count(); i++){
        connect(vector[i], &QToolButton::clicked,[this, i](){
            vector[i]->setUnreadCount(0);
            FriendIsShow[i] = true;
            qStack->setCurrentIndex(i); // 切换对话框
            ui->label->setText(this->vector[i]->text());
            qDebug() << "Button clicked:" << i << vector[i]->text();

        });
    }

}


   // 复制widget及其布局
QWidget* Menu2::copyWidget(QWidget* widget) {
    QWidget* newWidget = new QWidget();
    newWidget->setGeometry(widget->geometry());
    newWidget->setStyleSheet(widget->styleSheet());

    if (widget->layout()) {
        QVBoxLayout* layout = new QVBoxLayout();
        newWidget->setLayout(layout);

        for (int i = 0; i < widget->layout()->count(); ++i) {
            QWidget* originalChild = widget->layout()->itemAt(i)->widget();
            if (originalChild) {
                QWidget* newChild = copyWidget(originalChild);
                layout->addWidget(newChild);
            }
        }
    }

    return newWidget;
}

void Menu2::deleteAccont(){
    sendMsg(fd, AccountDelete, m_id);
    QMessageBox::information(this, "提示", "您的账号已注销！");
    this->close();
}
void Menu2::exit(){
    sendMsg(fd, Disconnent, m_id);
    this->close();
}

Menu2::~Menu2()
{
    // 确保删除所有动态分配的对象
    for (auto &btn : vector) {
        delete btn;
    }
    vector.clear();

    // 清理qStack中的页面
    while (qStack->count() > 0) {
        QWidget *w = qStack->widget(0);
        qStack->removeWidget(w);
        delete w;
    }
    delete setting;
    delete layout;
    // 删除qStack
    delete qStack;
    // close(fd);
    sendMsg(fd,Logout,"1");
    delete ui;
}
