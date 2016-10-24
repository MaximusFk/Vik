#include "dialogframe.h"
#include "ui_dialogframe.h"
#include <QDate>

DialogFrame::DialogFrame(std::shared_ptr<User> user, std::shared_ptr<Message> message, QWidget *parent) :
    QFrame(parent),
    user(user),
    ui(new Ui::DialogFrame)
{
    ui->setupUi(this);
    ui->text->setText(QString::fromStdString(message->getText()));
    QDateTime date = QDateTime::fromTime_t(message->getDate());
    if(date.date() == QDateTime::currentDateTime().date())
    {
        QString time = QString::number(date.time().hour());
        time += ':';
        time += QString::number(date.time().minute());
        ui->label->setText(time);
    }
    else
    {
        QString timeD = QString::number(date.date().day());
        timeD += '.';
        timeD += QString::number(date.date().month());
        timeD += '.';
        timeD += QString::number(date.date().year());
        ui->label->setText(timeD);
    }
    QString name = QString::fromStdString(user->getFirstName());
    name += ' ';
    name += QString::fromStdString(user->getLastName());
    ui->user->setText(name);
}

DialogFrame::~DialogFrame()
{
    delete ui;
}


void DialogFrame::mouseReleaseEvent(QMouseEvent *event)
{
    emit click(user.get());
}
