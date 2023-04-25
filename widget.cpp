#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , isMoving(false)
    ,temAxisXMin(0)
    ,temAxisXMax(10)
    ,temAxisYMin(25)
    ,temAxisYMax(26)
    ,humAxisXMin(0)
    ,humAxisXMax(10)
    ,humAxisYMin(50)
    ,humAxisYMax(51)
    ,tmpAxisXIndex(0)
    ,humAxisXIndex(0)
    ,isLightOn(false)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);

    this->setWindowIcon(QIcon(":/images/logo.png"));

    connect(ui->btnMaximize, &QPushButton::clicked, this, &Widget::onBtnMaximizeClicked);

    connect(ui->btnMinimize, &QPushButton::clicked, this, &Widget::onBtnMinimizeClicked);
    connect(ui->btnClose, &QPushButton::clicked, this, &Widget::onBtnCloseClicked);


    QGraphicsDropShadowEffect *shadowEffert = new QGraphicsDropShadowEffect();
    shadowEffert->setBlurRadius(8);
    shadowEffert->setColor(QColor(0,0,0,255));
    shadowEffert->setOffset(0);
    ui->widget_main->setGraphicsEffect(shadowEffert);

    setShadowEffect(ui->widget_LED);
    setShadowEffect(ui->widget_RFID);
    setShadowEffect(ui->widget_TMP);
    setShadowEffect(ui->widget_HUM);


    mqttInit();
    chartsInit();

    connect(ui->button_LED, &QPushButton::clicked, this, [&](){
        if (isLightOn)
        {
            m_client->publish(QString("GX/windows"), "{\"command\": \"control\", \"LED\": \"off\"}");
            ui->button_LED->setStyleSheet("image: url(:/images/lights_off.svg);\
                                          border:none;");
            ui->widget_LED->setStyleSheet(".QWidget{ border-radius:6px;background-color: rgb(32, 127, 76);}");
            isLightOn = false;
        }
        else
        {
            m_client->publish(QString("GX/windows"), "{\"command\": \"control\", \"LED\": \"on\"}");
            ui->button_LED->setStyleSheet("image: url(:/images/lights_on.svg);\
                                          border:none;");
            ui->widget_LED->setStyleSheet(".QWidget{ border-radius:6px;background-color: #f9f1db;}");
            isLightOn = true;
        }

        if (m_client->state() == QMqttClient::Disconnected)
        {
            m_client->connectToHost();
        }
    });
}

Widget::~Widget()
{
    delete ui;
}

void Widget::chartsInit()
{
    // 坐标
    axisXTmp = new QValueAxis();
    axisYTmp = new QValueAxis();
    axisXTmp->setMin(temAxisXMin);
    axisXTmp->setMax(temAxisXMax);
    axisYTmp->setMin(temAxisYMin);
    axisYTmp->setMax(temAxisYMax);
    axisXTmp->setTickCount(11);
    axisXTmp->setLabelFormat("%d");

    axisXHum = new QValueAxis();
    axisYHum = new QValueAxis();
    axisXHum->setMin(humAxisXMin);
    axisXHum->setMax(humAxisXMax);
    axisYHum->setMin(humAxisYMin);
    axisYHum->setMax(humAxisYMax);
    axisXHum->setTickCount(11);
    axisXHum->setLabelFormat("%d");
    // 曲线
    seriesTmp = new QLineSeries();
    seriesTmp->setName("温度");
    seriesTmp->setPointsVisible(true);

    seriesHum = new QLineSeries();
    seriesHum->setName("湿度");
    seriesHum->setPointsVisible(true);

    /// 图表
    // 温度
    chartForTmp = new QChart();
    chartForTmp->addAxis(axisXTmp, Qt::AlignBottom);
    chartForTmp->addAxis(axisYTmp, Qt::AlignLeft);
    chartForTmp->setTheme(QChart::ChartThemeQt);
    chartForTmp->setAnimationOptions(QChart::AllAnimations);
    chartForTmp->setTitle("温度变化曲线");
    chartForTmp->addSeries(seriesTmp);
    chartForTmp->legend()->hide();
    // 湿度
    chartForHum = new QChart();
    chartForHum->addAxis(axisXHum, Qt::AlignBottom);
    chartForHum->addAxis(axisYHum, Qt::AlignLeft);
    chartForHum->setTheme(QChart::ChartThemeLight);
    chartForHum->setAnimationOptions(QChart::AllAnimations);
    chartForHum->setTitle("相对湿度变化曲线");
    chartForHum->addSeries(seriesHum);
    chartForHum->legend()->hide();
    // Widget
    ui->widget_chartsTmp->setChart(chartForTmp);
    ui->widget_chartsTmp->setRenderHint(QPainter::Antialiasing);
    ui->widget_chartsHum->setChart(chartForHum);
    ui->widget_chartsHum->setRenderHint(QPainter::Antialiasing);
    // fix: 修复更新图表后存在残留(性能存疑)
    ui->widget_chartsTmp->setViewportUpdateMode(QChartView::FullViewportUpdate);
    ui->widget_chartsHum->setViewportUpdateMode(QChartView::FullViewportUpdate);
    /// 添加坐标轴
    // 温度
    seriesTmp->attachAxis(axisXTmp);
    seriesTmp->attachAxis(axisYTmp);
    // 湿度
    seriesHum->attachAxis(axisXHum);
    seriesHum->attachAxis(axisYHum);
}

void Widget::analysisJson(void)
{
    QJsonObject jsonObj = m_jsonDocument.object();
    QString command = jsonObj["command"].toString();
    qDebug() << "command:" << command;

    if ("report" == command)
    {
        if (jsonObj.contains("status"))
        {
            //            qDebug() << jsonObj["status"];
            QJsonObject statusObj = jsonObj["status"].toObject();
            qDebug() << statusObj;
            // LED
            if (statusObj.contains("LED") && ("on" == statusObj["LED"].toString()))
            {
                ui->button_LED->setStyleSheet("image: url(:/images/lights_on.svg);\
                                        border:none;");
                ui->widget_LED->setStyleSheet(".QWidget{ border-radius:6px;background-color: #f9f1db;}");
                isLightOn = true;

            }
            else if (statusObj.contains("LED") && ("off" == statusObj["LED"].toString()))
            {
                ui->button_LED->setStyleSheet("image: url(:/images/lights_off.svg);\
                                        border:none;");
                ui->widget_LED->setStyleSheet(".QWidget{ border-radius:6px;background-color: rgb(32, 127, 76);}");
                isLightOn = false;
            }

            // RFID
            if (statusObj.contains("rfid"))
            {
                if (statusObj["rfid"].isArray())
                {
                    m_rfidArray = statusObj["rfid"].toArray();
                    updateRfid();
                }
            }

            // 温湿度
            if (statusObj.contains("tmp") && statusObj.contains("hum"))
            {
                updateCharts(statusObj["tmp"].toDouble(), statusObj["hum"].toDouble());
            }
        }
    }
}

// 更新rfid
void Widget::updateRfid(void)
{
    if (0 == m_rfidArray.size())
    {
        return ;
    }

    ui->widget_RfidList->clear();

    for (auto i = m_rfidArray.begin(); i != m_rfidArray.end(); ++i)
    {
        ui->widget_RfidList->addItem(i->toObject()["message"].toString());
    }

}

void Widget::updateCharts(double tmp, double hum)
{
    ui->label_tmp->setText("当前室内温度：" + QString::number(tmp) + "℃");
    ui->label_hum->setText("当前室内相对湿度：" + QString::number(hum) + "%");

    // y轴缩放
    temAxisYMin = temAxisYMin > tmp-1 ? tmp - 1 : temAxisYMin;
    temAxisYMax = temAxisYMax < tmp+1 ? tmp + 1 : temAxisYMax;
    humAxisYMin = humAxisYMin > hum-1 ? hum - 1 : humAxisYMin;
    humAxisYMax = humAxisYMax < hum+1 ? hum + 1 : humAxisYMax;
    axisYTmp->setMin(temAxisYMin);
    axisYTmp->setMax(temAxisYMax);
    axisYHum->setMin(humAxisYMin);
    axisYHum->setMax(humAxisYMax);

    // 复位
    if (tmpAxisXIndex > 100)
    {
        seriesTmp->clear();
        tmpAxisXIndex = 0;
        temAxisXMin = 0;
        temAxisXMax = 10;

        seriesHum->clear();
        humAxisXIndex = 0;
        humAxisXMin = 0;
        humAxisXMax = 10;
    }

    // x轴位移
    if (tmpAxisXIndex > temAxisXMax)
    {
        temAxisXMin += 5;
        temAxisXMax += 5;

        humAxisXMin += 5;
        humAxisXMax += 5;
    }

    if(!this->isMaximized())
    {
        axisXTmp->setMin(temAxisXMin);
        axisXTmp->setMax(temAxisXMax);
        axisXHum->setMin(humAxisXMin);
        axisXHum->setMax(humAxisXMax);
    }

    seriesTmp->append(tmpAxisXIndex, tmp);
    seriesHum->append(humAxisXIndex, hum);

    tmpAxisXIndex++;
    humAxisXIndex++;
}

void Widget::mqttInit()
{
    m_client = new QMqttClient(this);
    m_client->setHostname("mqtt.gwf.icu");
    m_client->setPort(1883);
    m_client->setClientId("win" + QUuid::createUuid().toString());
    m_client->connectToHost();
    m_client->setAutoKeepAlive(true);

    connect(m_client, &QMqttClient::connected, this, [&](){
        qDebug() << "连接MQTT服务器成功";
        m_client->subscribe(QString("GX/rpi"));
        //        ui->label_iconMqtt->setVisible(true);
        connect(m_client, &QMqttClient::messageReceived, this, &Widget::mqttReceive);
        m_client->publish(QString("GX/windows"), "{\"command\":\"get\"}");
    });

    connect(m_client, &QMqttClient::disconnected, this, [&](){
        qDebug() << "MQTT连接已断开";
        //        ui->label_iconMqtt->setVisible(false);
    });

}

void Widget::mqttReceive(const QByteArray &message, const QMqttTopicName &topic)
{
    Q_UNUSED(topic)

    m_jsonDocument = QJsonDocument::fromJson(message);
    //    qDebug() << m_jsonDocument;
    m_client->publish(QString("GX/windows"), "{\"command\":\"ack\"}");
    analysisJson();
}

void Widget::onBtnMaximizeClicked()
{
    if (Qt::WindowMaximized == this->windowState())
    {
        ui->backgroundLayout->setMargin(9);
        ui->btnMaximize->setStyleSheet("border-image: url(:/images/maximize.svg);");
        this->showNormal();

        seriesTmp->setPointsVisible(true);
        seriesHum->setPointsVisible(true);

        axisXTmp->setMin(temAxisXMin);
        axisXTmp->setMax(temAxisXMax);
        axisXTmp->setTickCount(11);
        axisXHum->setMin(temAxisXMin);
        axisXHum->setMax(humAxisXMax);
        axisXHum->setTickCount(11);
    }
    else
    {
        ui->backgroundLayout->setMargin(0);
        ui->btnMaximize->setStyleSheet("border-image: url(:/images/normal.svg);");
        this->showMaximized();

        seriesTmp->setPointsVisible(false);
        seriesHum->setPointsVisible(false);

        axisXTmp->setMin(0);
        axisXTmp->setMax(100);
        axisXTmp->setTickCount(21);
        axisXHum->setMin(0);
        axisXHum->setMax(100);
        axisXHum->setTickCount(21);
    }
}

void Widget::onBtnMinimizeClicked()
{
    this->showMinimized();

}

void Widget::onBtnCloseClicked()
{
    qApp->exit(0);
}

void Widget::setShadowEffect(QWidget *widget)
{
    QGraphicsDropShadowEffect * effect = new QGraphicsDropShadowEffect(widget);
    effect->setOffset(5, 5);//设置阴影距离
    effect->setColor(QColor(0,0,0,90));//设置阴影颜色
    effect->setBlurRadius(7);//设置阴影圆角
    widget->setGraphicsEffect(effect);
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    if (Qt::LeftButton == event->button())
    {
        m_windowPos = this->pos();
        m_mousePos = event->globalPos();
        isMoving = true;
    }
    QWidget::mousePressEvent(event);
}

void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    if (Qt::LeftButton == event->button())
    {
        isMoving = false;
    }
    QWidget::mouseReleaseEvent(event);
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton  && isMoving && (this->windowState() != Qt::WindowMaximized))
    {
        this->move(m_windowPos - (m_mousePos - event->globalPos()));

        m_windowPos = this->pos();
        m_mousePos = event->globalPos();
    }

    QWidget::mouseReleaseEvent(event);
}

