#include "widget.h"
#include "log.h"
#include "client.h"
#include "menu.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 设置语言
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "chatroom_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    Client cli(argc,argv);

    // log b;
    // b.show();
    // menu *w = new menu(nullptr,1);
    // w->show();
    // Widget w;
    // w.show();
    return a.exec();
}
