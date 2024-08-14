#ifndef CREATEFRIENDDIALOG_H
#define CREATEFRIENDDIALOG_H

#include <QApplication>
#include <QWidget>
#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QMessageBox>

class CreateFriendDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CreateFriendDialog(QWidget *parent,
                                const std::unordered_map<std::string,std::string> & friends);
    std::vector<std::string> getSelectedFriends() const;

private:
    QListWidget *friendListWidget;
    QPushButton *deleteBtn;
    std::unordered_map<std::string, std::string> friendList;

private slots:
    void onDeleteBtnClicked();
};

#endif // CREATEFRIENDDIALOG_H
