#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "mysocket.h"
#include <QDebug>
#include "createfrienddialog.h"
#include "filemenu.h"

using Json = nlohmann::json;

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent,QString uname, QString uid, QString id, QString name,int sfd);
    Widget(QWidget *parent,QString gname, QString gid, QString id, QString name,int sfd,
           std::unordered_map<std::string, std::string> list);
    QString getName();// 获取名字
    QString getMsg();// 获取信息
    void SendMsg();
    void prints(std::string time, std::string msg, int flag);
    void printinfo(std::string msg);
    void adduser(std::string id, std::string name);
    void deleteuser(std::string id, std::string name);
    void banneduser();
    ~Widget();
signals:
    //关闭窗口信号
    void closeWidget();
    void readmsg(std::string id);
    void fileinfos(std::string filename);
    void sendf(std::string filepath, std::string uid);
    void recvf(std::string filename);
public slots:
    void addmember(std::string id, std::string name);
    void inithistory(std::vector<std::string> ll);
    void getData(std::string data);

    void sentFile(std::string path);
    // void recvFile(std::string filename);
    void recvFileask(std::string filename);
    void fileinfo(std::string fileinfo);

    void limitText();

private:

    Ui::Widget *ui;
    CreateFriendDialog *w;
    FileMenu *f;
    std::unordered_map<std::string,std::string> members;
    // std::unordered_map<std::string, std::string> friends;
    QString m_name;
    QString m_id;
    QString u_id;
    QString u_name;
    int fd;
public:
    // 重写关闭事件
    void closeEvent(QCloseEvent*);
};
#endif // WIDGET_H
