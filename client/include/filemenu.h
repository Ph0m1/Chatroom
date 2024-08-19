#ifndef FILEMENU_H
#define FILEMENU_H

#include <QPushButton>
#include <QMessageBox>
#include <QWidget>
#include <QStringListModel>
#include <QFileDialog>
#include <QDebug>

namespace Ui {
class FileMenu;
}

class FileMenu : public QWidget
{
    Q_OBJECT

public:
    explicit FileMenu(QWidget *parent = nullptr);
    ~FileMenu();

    std::string getFile();
public slots:
    void addFilelist(std::string filename);
    void setFilelist(std::vector<std::string> filenames);
    void onItemDoubleClicked(const QModelIndex & index);

signals:
    void filePath(std::string path);
    void seletedfilename(std::string filename);
private:
    Ui::FileMenu *ui;
    QStringList *list;
    QStringListModel *model;
};

#endif // FILEMENU_H
