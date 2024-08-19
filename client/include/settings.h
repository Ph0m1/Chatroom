#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>

namespace Ui {
class Settings;
}

class Settings : public QWidget
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();

signals:
    void deleteAccont();
    void exit();
public slots:

private:
    Ui::Settings *ui;
};

#endif // SETTINGS_H
