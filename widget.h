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
#include <QtCharts>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMargins>
#include <QUuid>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;

    bool isMoving;
    QPoint m_windowPos;
    QPoint m_mousePos;

    double temAxisXMin;
    double temAxisXMax;
    double temAxisYMin;
    double temAxisYMax;
    double humAxisXMin;
    double humAxisXMax;
    double humAxisYMin;
    double humAxisYMax;

    size_t tmpAxisXIndex;
    size_t humAxisXIndex;

    QChart *chartForTmp;
    QChart *chartForHum;
    QSplineSeries *seriesTmp;  // 温度曲线
    QSplineSeries *seriesHum;   // 湿度曲线
    QValueAxis *axisXTmp;
    QValueAxis *axisYTmp;
    QValueAxis *axisXHum;
    QValueAxis *axisYHum;

    void chartsInit(void);
    void analysisJson(void);    // 解析JSon
    void updateRfid(void);    // 更新rfid
    void updateCharts(double tmp, double hum);    // 更新图表

    QMqttClient *m_client;
    bool hasMqttClient;
    QJsonDocument m_jsonDocument;
    QJsonArray m_rfidArray;
    bool isLightOn;

    void mqttInit(void);
    void mqttReceive(const QByteArray &message, const QMqttTopicName &topic);

    void onBtnMaximizeClicked(void);
    void onBtnMinimizeClicked(void);
    void onBtnCloseClicked(void);

    void setShadowEffect(QWidget *widget);

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
};
#endif // WIDGET_H
