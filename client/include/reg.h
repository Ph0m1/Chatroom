#ifndef REG_H
#define REG_H

#include "mysocket.h"
#include <QWidget>

namespace Ui {
class reg;
}

class reg : public QWidget
{
    Q_OBJECT

public:
    explicit reg(QWidget *parent,int sfd, int port, std::string ip);
    ~reg();

private:
    Ui::reg *ui;
    int port = 9098;
    std::string ip = "127.0.0.1";
};

#endif // REG_H
