#ifndef DIALOGFRAME_H
#define DIALOGFRAME_H

#include <QFrame>
#include <message.h>
#include <user.h>

namespace Ui {
class DialogFrame;
}

class DialogFrame : public QFrame
{
    Q_OBJECT

    std::shared_ptr<User> user;

public:
    explicit DialogFrame(std::shared_ptr<User> user, std::shared_ptr<Message> message, QWidget *parent = 0);
    ~DialogFrame();

public slots:

signals:
    void click(User *);

private:
    Ui::DialogFrame *ui;

    // QWidget interface
protected:
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif // DIALOGFRAME_H
