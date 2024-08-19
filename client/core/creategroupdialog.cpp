#include "creategroupdialog.h"

CreateGroupDialog::CreateGroupDialog(QWidget *parent,
                                     const std::unordered_map<std::string, std::string> &friends)
    :QDialog(parent)
    ,friendList(friends)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    friendListWidget = new QListWidget(this);
    nameEdit = new QLineEdit(this);
    nameEdit->setPlaceholderText("请输入群聊名称...");

    for(const auto & fp : friendList){
        QListWidgetItem * item = new QListWidgetItem(QString::fromStdString(fp.second) + "<"
                                                    + QString::fromStdString(fp.first) + ">");
        item->setCheckState(Qt::Unchecked);
        friendListWidget->addItem(item);
    }

    creatBtn = new QPushButton("创建群聊", this);

    layout->addWidget(nameEdit);
    layout->addWidget(friendListWidget);
    layout->addWidget(creatBtn);

    connect(creatBtn, &QPushButton::clicked, this, &CreateGroupDialog::onCreateBtnClicked);

}

std::string CreateGroupDialog::getGroupName(){
    if(nameEdit->text().isEmpty() || nameEdit->text() == "请输入群聊名称..."){
        QMessageBox::warning(this, "警告", "请输入群聊名称!");
        return " "; // 返回为空则为错误;
    }
    std::string data = nameEdit->text().toStdString();
    return data;
}

std::vector<std::string> CreateGroupDialog::getSelectedFriends() const{
    std::vector<std::string> selectedFriends;
    for(int i = 0; i<friendListWidget->count(); i++){
        QListWidgetItem *item = friendListWidget->item(i);
        if(item->checkState() == Qt::Checked){
            QString text = item->text();
            std::string id = text.toStdString();
            size_t pos = id.find('<');
            if(pos != std::string::npos){
                id = id.substr(pos+1,id.size()-pos-2);
                selectedFriends.push_back(id);
            }
        }
    }
    return selectedFriends;
}

void CreateGroupDialog::onCreateBtnClicked(){
    accept();
}
