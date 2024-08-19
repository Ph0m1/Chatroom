#include "informations.h"
#include "ui_informations.h"

informations::informations(QWidget *parent, int fd, std::vector<std::string> ll, int flag)
    : QWidget(parent)
    , ui(new Ui::informations)
    , fd(fd)
{
    ui->setupUi(this);
    mainLayout = new QVBoxLayout(this);
    if(flag == 0){
        for(auto & id : ll){
            addfriendRow(id);
        }
    }
    else if(flag == 1){
        for(auto&t : ll){
            addgroupRow(t);
        }
    }
}

void informations::removeRow(QWidget* rowWidget){

    // 删除行布局中的所有小部件
    QLayout* rowLayout = rowWidget->layout();
    while (QLayoutItem* item = rowLayout->takeAt(0)) {
        if (QWidget* widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }
    // 删除行布局和行小部件
    mainLayout->removeWidget(rowWidget);
    delete rowWidget;
}

void informations::addgroupRow(std::string info){
    QHBoxLayout* rowLayout = new QHBoxLayout;
    QWidget* rowWidget = new QWidget(this);
    QLabel* label = new QLabel(static_cast<QString>(info.c_str()),this);// 展示信息
    QPushButton* yesbtn = new QPushButton("同意",this);
    QPushButton* nobtn = new QPushButton("忽略",this);

    connect(yesbtn, &QPushButton::clicked, this, [=](){
        removeRow(rowWidget);
        sendMsg(fd, GroupJoinYes, info);
    });
    connect(nobtn, &QPushButton::clicked, this, [=](){
        removeRow(rowWidget);
        sendMsg(fd, GroupJoinNo, info);
    });

    rowLayout->addWidget(label);
    rowLayout->addWidget(yesbtn);
    rowLayout->addWidget(nobtn);


    rowWidget->setLayout(rowLayout);
    mainLayout->addWidget(rowWidget);
}
void informations::addfriendRow(std::string info)
{
    QHBoxLayout* rowLayout = new QHBoxLayout;
    QWidget* rowWidget = new QWidget(this);
    QLabel* label = new QLabel(static_cast<QString>(info.c_str())+"请求添加您为好友",this);
    QPushButton* yesbtn = new QPushButton("同意",this);
    QPushButton* nobtn = new QPushButton("忽略",this);

    connect(yesbtn, &QPushButton::clicked, this, [=](){
        removeRow(rowWidget);
        sendMsg(fd, FriendAddYes, info);
    });
    connect(nobtn, &QPushButton::clicked, this, [=](){
        removeRow(rowWidget);
        sendMsg(fd, FriendAddNo, info);
    });

    rowLayout->addWidget(label);
    rowLayout->addWidget(yesbtn);
    rowLayout->addWidget(nobtn);


    rowWidget->setLayout(rowLayout);
    mainLayout->addWidget(rowWidget);
}

informations::~informations()
{
    delete ui;
}

