#ifndef MESSAGEFRAME_H
#define MESSAGEFRAME_H

#include <QFrame>
#include <QLabel>
#include <message.h>

namespace Ui {
class MessageFrame;
}

class MessageFrame : public QFrame
{
    Q_OBJECT

    std::shared_ptr<Message> message;

    QLabel *text;

    explicit MessageFrame(QWidget *parent = 0);
public:
    MessageFrame(std::shared_ptr<Message> message, QWidget *parent);
    ~MessageFrame();

    bool isOut() const;

private:
    Ui::MessageFrame *ui;
};

#endif // MESSAGEFRAME_H
