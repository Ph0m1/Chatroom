#include "filemenu.h"
#include "ui_filemenu.h"

FileMenu::FileMenu(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FileMenu)
{
    ui->setupUi(this);

    list = new QStringList;
    model = new QStringListModel(this);
    model->setStringList(*list);
    ui->fileList->setModel(model);
    ui->fileList->setViewMode(QListView::ListMode);
    ui->fileList->setSelectionMode(QAbstractItemView::SingleSelection);


    connect(ui->choseFile, &QPushButton::clicked, this, [=](){
        std::string path = getFile();
        emit filePath(path);
    });
    connect(ui->cancelbtn, &QPushButton::clicked, this, [=](){
        this->close();
    });
    connect(ui->fileList, &QListView::doubleClicked, this, &FileMenu::onItemDoubleClicked);
}

FileMenu::~FileMenu()
{
    delete ui;
}

void FileMenu::addFilelist(std::string filename){
    list->append(QString::fromStdString(filename));
    model->setStringList(*list);
    ui->fileList->update();

    qDebug() << "setFilelist: " << QString::fromStdString(filename);  // 调试信息
    qDebug() << "Current list: " << *list;  // 查看列表当前内容
}

void FileMenu::setFilelist(std::vector<std::string> filenames){
    for(auto & filename : filenames){
        list->append(QString::fromStdString(filename));
    }
    model->setStringList(*list);
    ui->fileList->update();

    qDebug() << "setFilelist: " << QString::fromStdString(filenames[0]);  // 调试信息
    qDebug() << "Current list: " << *list;  // 查看列表当前内容
}

void FileMenu::onItemDoubleClicked(const QModelIndex & index){
    // 获取双击内容
    std::string filename = index.data(Qt::DisplayRole).toString().toStdString();
    std::string info = "是否要接收文件：" + filename;
    //提示
    QMessageBox::StandardButton reply;
    reply = QMessageBox::information(this, "提示", info.c_str(), QMessageBox::Cancel | QMessageBox::Ok);
    if(reply == QMessageBox::Ok){
        emit seletedfilename(filename);
    }
}

std::string FileMenu::getFile(){
    QString filepath = QFileDialog::getOpenFileName(this, "选择文件", "", "所有文件(*)");
    std::string path = " ";
    if(!filepath.isEmpty()){
        QMessageBox::information(this, "文件路径", "您选择的文件路径是：\n" + filepath);
        path = filepath.toStdString();
    }
    return path;
}
