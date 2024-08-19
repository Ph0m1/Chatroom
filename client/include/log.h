#ifndef LOG_H
#define LOG_H

#include <QWidget>
#include "mysocket.h"
namespace Ui {
class log;
}

class log : public QWidget
{
    Q_OBJECT

public:
    explicit log(QWidget *parent,QString id,int sfd, int port, std::string ip);
    explicit log(QWidget *parent,int sfd, int port, std::string ip);
    ~log();

private:
    Ui::log *ui;
    int fd;
    int port = 9098;
    std::string ip = "127.0.0.1";
};

#endif // LOG_H
