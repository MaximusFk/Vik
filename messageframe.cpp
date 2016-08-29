#include "messageframe.h"
#include "ui_messageframe.h"
#include <assert.h>
#include "audioframe.h"
#include "Log.h"
#include "photo.h"

#include <QGraphicsView>

MessageFrame::MessageFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::MessageFrame)
{
    ui->setupUi(this);

    assert(parent != nullptr);
    this->setMaximumWidth(parent->width() / 1.1f);
}

MessageFrame::MessageFrame(std::shared_ptr<Message> message, QWidget *parent) :
    MessageFrame(parent)
{
    int allHeight = 0;
    this->message = message;
    if(!this->message->getText().empty())
    {
        text = new QLabel(this);
        text->setText(QString::fromStdString(this->message->getText()));
        text->setWordWrap(true);
        if(this->message->isOut())
        {
            this->text->move(this->width() - text->width(), text->y());
        }
        allHeight += text->height();
        if(!this->message->isRead())
        {
            QPalette palette;
            palette.setColor(QPalette::ColorRole::BrightText, QColor(75, 0, 130, 255));
            this->setPalette(palette);
            this->update();
        }
    }
    MessageAttachments a = this->message->getMessageAttachments();
    Log::i(std::to_string(a.size()).c_str());
    if(a.size() > 0)
    {
        Log::i("entering to AudioFrameConstructor MessageFrame");
        AudioFrame * last = nullptr;
        for(int i = 0; i < a.size(); ++i)
        {
            if(a[i])
            {
                switch (a[i]->getType()) {
                case ObjectType::AUDIO:
                {
                    std::shared_ptr<Audio> audio = std::dynamic_pointer_cast<Audio>(a[i]);
                    AudioFrame * frame = new AudioFrame(audio, this);
                    frame->show();
                    if(last != nullptr)
                        frame->move(0, (allHeight += last->height() + last->y() + 2));
                    else
                        frame->move(0, allHeight);
                    allHeight += frame->height();
                }
                    break;
                case ObjectType::PHOTO:
                {
                    std::shared_ptr<Photo> photo = std::dynamic_pointer_cast<Photo>(a[i]);
                }
                    break;
                default:
                    break;
                }

            }
        }
    }
    this->setMinimumHeight(allHeight);
}

MessageFrame::~MessageFrame()
{
    delete ui;
}

bool MessageFrame::isOut() const
{
    return this->message->isOut();
}
