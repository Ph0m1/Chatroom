#ifndef BADGETOOLBUTTON_H
#define BADGETOOLBUTTON_H
#include <QApplication>
#include <QToolButton>
#include <QPainter>
#include <QStyleOptionToolButton>

class BadgeToolButton : public QToolButton{
    Q_OBJECT

public:
    BadgeToolButton(QWidget* parent = nullptr)
        : QToolButton(parent)
        , m_unreadCount(0)
    {
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        setFixedHeight(25); // 设置固定高度
        setMinimumWidth(30);
    }

    void setUnreadCount(int count){
        m_unreadCount = count;
        update();// 触发重绘
    }

    void addUnreadCount(int c){
        m_unreadCount += c;
        update();
    }

protected:
    void paintEvent(QPaintEvent* event) override{
        QToolButton::paintEvent(event);
        if(m_unreadCount > 0){
            QPainter painter(this);
            painter.setRenderHint(QPainter::Antialiasing);

            int badgeSize = 15;
            int badgeX = width() - badgeSize - 2;
            int badgeY = 4;

            //绘制红色圆圈
            painter.setBrush(Qt::red);
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(badgeX, badgeY, badgeSize, badgeSize);

            //绘制未读消息数
            painter.setPen(Qt::white);
            painter.setFont(QFont("Arial", 10, QFont::Bold));
            painter.drawText(QRect(badgeX, badgeY, badgeSize, badgeSize), Qt::AlignCenter, QString::number(m_unreadCount));
            // painter.end();
        }
    }

private:
    int m_unreadCount;
};


#endif // BADGETOOLBUTTON_H
