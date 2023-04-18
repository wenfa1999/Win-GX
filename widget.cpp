#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);

    connect(ui->btnMaximize, &QPushButton::clicked, this, &Widget::onBtnMaximizeClicked);

    connect(ui->btnMinimize, &QPushButton::clicked, this, &Widget::onBtnMinimizeClicked);


    connect(ui->btnClose, &QPushButton::clicked, this, &Widget::onBtnCloseClicked);

    QGraphicsDropShadowEffect *shadowEffert = new QGraphicsDropShadowEffect();
    shadowEffert->setBlurRadius(8);
    shadowEffert->setColor(QColor(0,0,0,255));
    shadowEffert->setOffset(0);
    ui->widget_main->setGraphicsEffect(shadowEffert);

}

Widget::~Widget()
{
    delete ui;
}

void Widget::onBtnMaximizeClicked()
{
    if (Qt::WindowNoState == this->windowState())
    {
        ui->verticalLayout->setMargin(0);
        ui->btnMaximize->setStyleSheet("border-image: url(:/images/normal.svg);");
        this->showMaximized();
    }
    else if (Qt::WindowMaximized == this->windowState())
    {
        ui->verticalLayout->setMargin(9);
        ui->btnMaximize->setStyleSheet("border-image: url(:/images/maximize.svg);");
        this->showNormal();
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

void Widget::mousePressEvent(QMouseEvent *event)
{
    if (Qt::LeftButton == event->button())
    {
        m_windowPos = this->pos();
        m_mousePos = event->globalPos();
    }
    QWidget::mousePressEvent(event);
}

void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        this->move(m_windowPos - (m_mousePos - event->globalPos()));
        m_windowPos = this->pos();
        m_mousePos = event->globalPos();
    }

    QWidget::mouseReleaseEvent(event);
}

