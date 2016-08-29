#include "userframe.h"
#include "ui_userframe.h"

UserFrame::UserFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::UserFrame)
{
    ui->setupUi(this);
}

UserFrame::~UserFrame()
{
    delete ui;
}
