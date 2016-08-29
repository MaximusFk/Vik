#ifndef DIALOGFRAME_H
#define DIALOGFRAME_H

#include <QFrame>
#include <message.h>
#include <user.h>

#include "updatethread.h"

namespace Ui {
class DialogFrame;
}

class DialogFrame : public QFrame
{
    Q_OBJECT

    UpdateThread * updater;
    std::shared_ptr<User> user;

public:
    explicit DialogFrame(std::shared_ptr<Message> message, QWidget *parent = 0);
    ~DialogFrame();

public slots:
    void finished();

private:
    Ui::DialogFrame *ui;
};

#endif // DIALOGFRAME_H
