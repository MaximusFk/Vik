#include "loginwidget.h"
#include "ui_loginwidget.h"

LoginWidget::LoginWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginWidget)
{
    ui->setupUi(this);
}

LoginWidget::LoginWidget(const char *addr, QWidget *parent) :
    LoginWidget(parent)
{
    login = new QWebEngineView(this);
    login->setMaximumSize(this->maximumSize());
    login->load(QUrl(QString(addr)));
    connect(login, &QWebEngineView::urlChanged, this, &LoginWidget::on_login_urlChanged);
}

LoginWidget::~LoginWidget()
{
    delete ui;
    login->deleteLater();
    login = nullptr;
}

//void LoginWidget::urlChanged(const QUrl & url)
//{

//}

void LoginWidget::on_login_urlChanged(const QUrl &arg1)
{
    emit urlChanged(arg1);
}
