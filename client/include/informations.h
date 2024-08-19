#ifndef INFORMATIONS_H
#define INFORMATIONS_H

#include <QWidget>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QLabel>
#include <QPushButton>


#include "mysocket.h"
#include "msgtype.h"

namespace Ui {
class informations;
}

class informations : public QWidget
{
    Q_OBJECT

public:
    explicit informations(QWidget *parent, int fd, std::vector<std::string> ll, int flag);
    ~informations();

signals:
    // void remove();
public slots:
    // void showinfos(std::vector<std::string>);
    void addgroupRow(std::string info);
    void addfriendRow(std::string info);
    void removeRow(QWidget* rowWidget);
private:
    Ui::informations *ui;
    std::vector<std::string> infos;
    int fd;
    QVBoxLayout* mainLayout;

    // void addRow(const std::string &info);
};

#endif // INFORMATIONS_H
