#ifndef SETNEWPASSWD_H
#define SETNEWPASSWD_H

#include "mysocket.h"
#include <QWidget>

namespace Ui {
class SetNewPasswd;
}

class SetNewPasswd : public QWidget
{
    Q_OBJECT

public:
    explicit SetNewPasswd(QWidget *parent,int sfd, int port, std::string ip);
    ~SetNewPasswd();

private:
    Ui::SetNewPasswd *ui;
    int fd;
    int port = 9098;
    std::string ip = "127.0.0.1";
};

#endif // SETNEWPASSWD_H
