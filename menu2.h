#ifndef MENU2_H
#define MENU2_H

#include <QWidget>
#include <QToolButton>
#include <QPixmap>
#include <QMap>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QStackedLayout>
#include <QStackedWidget>
#include <QDebug>

#include "settings.h"
#include "threadpool.h"
#include "badgetoolbutton.h"
#include "informations.h"
#include "creategroupdialog.h"
#include "createfrienddialog.h"

#include "widget.h"

namespace Ui {
class Menu2;
}

class Menu2 : public QWidget
{
    Q_OBJECT

public:
    explicit Menu2(QWidget *parent, int sfd,const std::string &data, int port, std::string ip);
    ~Menu2();
private:

    void resetFbtn(const std::string &str);
    void setFbtn(std::unordered_map<std::string,std::string> list, int flag,
                    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> gmm);
    void setMbtn(std::unordered_map<std::string,std::string> list);
    void printmsg(std::string user, std::string msg, int flag, Widget* w);
    // 复制widget及其布局
    QWidget* copyWidget(QWidget* widget);


    void createGroup(const std::vector<std::string> &ll, const std::string & name);
    void deletefriends(const std::vector<std::string> &ll);
    void readFromServer(int fd);

    void sendfileinfo(std::string fileinfo);

    ThreadPool *threadPool;

private:
    Ui::Menu2 *ui;
    int fd;
    int port = 9098;
    std::string ip = "127.0.0.1";

    std::string m_id;
    std::string m_name;
    std::vector<std::string> friendaddlist;// 好友申请列表
    std::vector<std::string> groupinfolist;// 群通知按钮

    Settings *setting = new Settings;
    informations *w; // 好友申请处理界面
    informations *w2; // 群通知处理界面
    BadgeToolButton *friendaddbtn;
    BadgeToolButton * groupinfobtn;
    QVBoxLayout *layout;
    QVector<bool> FriendIsShow;
    QVector<bool> GroupIsShow;
    QVector<bool> MsgIsShow;
    QVector<bool> MsgIsOn;
    QVector<Widget*> ww;
    bool btnIsChecked[2];

    std::mutex pauseMutex;
    std::condition_variable pauseCondition;
    bool pauseThread = false;

    QVector<BadgeToolButton*> vector;// 按钮
    std::unordered_map<std::string, BadgeToolButton*> lists; // 按钮
    std::unordered_map<std::string, std::string> friendlist;
    std::unordered_map<std::string, std::string> grouplist;
    QStackedWidget *qStack = new QStackedWidget(this); // 聊天窗口
    // QStackedLayout *listStack = new QStackedLayout(this); // 消息/好友列表
signals:
    void sendinfos(std::string ll);
    void sendlist(std::vector<std::string> ll);
    void sendData(std::string msg);
    void friendsg(std::string msg);
    void refreshFriendList(std::string list);
    void friendaddmsg(std::string str);
    void addRowList(std::string str);
    void addgroupRow(std::string str);
    void receivedfile(std::string fileinfo);
    void fileinfos(std::string fileinfo);
    // void newMsg(std::string id);
public slots:
    void updatefriendaddbtn(std::string id);
    void updateList(std::string id);
    void friendAdd(std::string msg);
    void updateFriendList(std::string list);
    void deleteAccont();
    void exit();

    void pause();
    void resume();

    // void fileinfo(std::string fileinfo);
    void recvfile(std::string fileinfo);

    void sendFile(const std::string &filepath, std::string uid);
private slots:
    void showCreateGroupDialog();
    void showDeleteFriendDialog();
};

class FileSocket{
public:
    FileSocket() = default;
    ~FileSocket() = default;
    FileSocket(std::string ip, int port){
        this->fd = socket(AF_INET, SOCK_STREAM, 0);
        if(this->fd < 0){
            std::cerr << "File socket error!" << std::endl;
            return;
        }
        struct sockaddr_in addr;
        memset(&addr,0,sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        if(inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0){
            std::cout << "Invalid address/ Address not supported" << std::endl;
            return;
        }
        if(connect(this->fd,(struct sockaddr*)&addr,sizeof(addr)) < 0){ //链接服务端
            std::cout<<"connect error"<<std::endl;
            return;
        }
    }

public:
    int getfilefd(){
        return this->fd;
    }
private:
    int port = 9098;
    std::string ip = "127.0.0.1";
    int fd;
};
#endif // MENU2_H
