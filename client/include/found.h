#ifndef FOUND_H
#define FOUND_H

#include <QWidget>
#include "mysocket.h"
namespace Ui {
class Found;
}

class Found : public QWidget
{
    Q_OBJECT

public:
    explicit Found(QWidget *parent, int sfd, int port, std::string ip);
    explicit Found(QWidget *parent, int sfd, int tmp);
    ~Found();

private:
    Ui::Found *ui;
    int fd;

    int port = 9098;
    std::string ip = "127.0.0.1";
};

#endif // FOUND_H
