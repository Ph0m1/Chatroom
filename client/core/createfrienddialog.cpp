#include "createfrienddialog.h"

CreateFriendDialog::CreateFriendDialog(QWidget *parent,
                                       const std::unordered_map<std::string, std::string> &friends)
    :QDialog(parent)
    ,friendList(friends)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    friendListWidget = new QListWidget(this);

    for(const auto & fp : friendList){
        QListWidgetItem * item = new QListWidgetItem(QString::fromStdString(fp.second) + "<"
                                                    + QString::fromStdString(fp.first) + ">");
        item->setCheckState(Qt::Unchecked);
        friendListWidget->addItem(item);
    }

    deleteBtn = new QPushButton("删除好友", this);

    layout->addWidget(friendListWidget);
    layout->addWidget(deleteBtn);

    connect(deleteBtn, &QPushButton::clicked, this, &CreateFriendDialog::onDeleteBtnClicked);
}

std::vector<std::string> CreateFriendDialog::getSelectedFriends() const{
    std::vector<std::string> selectedFriends;

    for(int i = 0; i < friendListWidget->count(); i++){
        QListWidgetItem *item= friendListWidget->item(i);
        if(item->checkState() == Qt::Checked){
            QString text = item->text();
            std::string id = text.toStdString();
            size_t pos =id.find('<');
            if(pos != std::string::npos){
                id = id.substr(pos+1, id.size()-pos-2);
                selectedFriends.push_back(id);
            }
        }
    }
    return selectedFriends;
}

void CreateFriendDialog::onDeleteBtnClicked(){
    accept();
}
