#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMqttClient>
#include <QtMqtt/QMqttClient>
#include <QPushButton>
#include <QHBoxLayout>
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QPainterPath>
#include <QMouseEvent>
#include <QLabel>
#include <QPoint>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;

    void onBtnMaximizeClicked();
    void onBtnMinimizeClicked();
    void onBtnCloseClicked();


    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    QPoint m_windowPos;
    QPoint m_mousePos;
};
#endif // WIDGET_H
