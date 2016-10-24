#include "messageframe.h"
#include "ui_messageframe.h"
#include <assert.h>
#include "audioframe.h"
#include "Log.h"
#include "photo.h"

#include <QGraphicsView>
#include <QDate>

#include "contentframe.h"

MessageFrame::MessageFrame(QWidget *parent) :
    QFrame(parent),
    text(nullptr),
    audioContent(nullptr),
    ui(new Ui::MessageFrame)
{
    ui->setupUi(this);

    assert(parent != nullptr);
    this->setMaximumWidth(parent->width() / 1.1f);
}

MessageFrame::MessageFrame(std::shared_ptr<Message> message, QWidget *parent) :
    MessageFrame(parent)
{
    QSize baseSize(0, 0);
    baseSize.rheight() += ui->time->height();
    this->message = message;
    QDateTime md = QDateTime::fromTime_t(message->getDate());
    QString time;
    time = QString::number(md.time().hour());
    time += ':';
    time += QString::number(md.time().minute());
    if(md.date() != QDateTime::currentDateTime().date())
    {
        time += ' ';
        time += QString::number(md.date().day());
        time += '.';
        time += QString::number(md.date().month());
        time += '.';
        time += QString::number(md.date().year());
    }
    ui->time->setText(time);
    ui->time->setAlignment(Qt::AlignVCenter | (this->message->isOut() ? Qt::AlignRight : Qt::AlignLeft));
    if(!this->message->getText().empty())
    {
        text = new QLabel(this);
        text->setText(QString::fromStdString(this->message->getText()));
        text->setWordWrap(true);
        text->setAlignment(ui->time->alignment());
        text->setBaseSize(this->width(), text->height());
//        if(this->message->isOut())
//        {
//            this->text->move(this->width() - text->width(), text->y());
//        }
        baseSize.rheight() += text->height();
        if(!this->message->isRead())
        {
            QPalette palette;
            palette.setColor(QPalette::ColorRole::BrightText, QColor(75, 0, 130, 255));
            this->setPalette(palette);
            this->update();
        }
    }
    MessageAttachments a = this->message->getMessageAttachments();
    if(a.size() > 0)
    {
        Log::i("entering to Attachments");
        for(int i = 0; i < a.size(); ++i)
        {
            if(a[i])
            {
                switch (a[i]->getType()) {
                case ObjectType::AUDIO:
                {
                    Log::i("Audio");
                    if(audioContent == nullptr)
                        audioContent = new AudioContentFrame(this);
                    std::shared_ptr<Audio> audio = std::dynamic_pointer_cast<Audio>(a[i]);
                    AudioFrame * frame = new AudioFrame(audio, audioContent);
                    audioContent->addFrame(frame);
                    audioContent->move(0, (this->text != nullptr ? this->text->height() : 0));
                    frame->show();
                    audioContent->show();
                }
                    break;
                case ObjectType::PHOTO:
                {
                    Log::i("Photo");
                    std::shared_ptr<Photo> photo = std::dynamic_pointer_cast<Photo>(a[i]);
                }
                    break;
                default:
                    break;
                }

            }
        }
        Log::i("leaving Attachments");
        if(audioContent)
            baseSize.rheight() += audioContent->height();
    }
    ui->time->move(0, baseSize.height() - ui->time->height());
    this->setMinimumHeight(baseSize.height());
    //this->setGeometry(this->x(), this->y(), baseSize.width(), baseSize.height());
}

MessageFrame::~MessageFrame()
{
    delete ui;
}

bool MessageFrame::isOut() const
{
    return this->message->isOut();
}
