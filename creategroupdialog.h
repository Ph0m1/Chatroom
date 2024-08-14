#ifndef CREATEGROUPDIALOG_H
#define CREATEGROUPDIALOG_H

#include <QApplication>
#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QMessageBox>

class CreateGroupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateGroupDialog(QWidget *parent ,
                               const std::unordered_map<std::string, std::string> &friends);
    std::vector<std::string> getSelectedFriends() const;
    std::string getGroupName();

private:
    QListWidget *friendListWidget;
    QPushButton *creatBtn;
    QLineEdit *nameEdit;
    std::unordered_map<std::string, std::string> friendList;

private slots:
    void onCreateBtnClicked();
};

#endif // CREATEGROUPDIALOG_H
