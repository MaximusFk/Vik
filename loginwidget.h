#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>
#include <QWebEngineView>

namespace Ui {
class LoginWidget;
}

class LoginWidget : public QWidget
{
    Q_OBJECT
    friend class MainWindow;

public:
    explicit LoginWidget(QWidget *parent = 0);
    LoginWidget(const char * addr, QWidget *parent = 0);
    ~LoginWidget();

signals:
    void urlChanged(const QUrl &);

private slots:
    void on_login_urlChanged(const QUrl &arg1);

private:
    Ui::LoginWidget *ui;
    QWebEngineView * login;
};

#endif // LOGINWIDGET_H
