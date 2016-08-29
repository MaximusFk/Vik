#ifndef USERFRAME_H
#define USERFRAME_H

#include <QFrame>

namespace Ui {
class UserFrame;
}

class UserFrame : public QFrame
{
    Q_OBJECT

public:
    explicit UserFrame(QWidget *parent = 0);
    ~UserFrame();

private:
    Ui::UserFrame *ui;
};

#endif // USERFRAME_H
