#include "contentframe.h"
#include "audioframe.h"

ContentFrame::ContentFrame(QWidget *parent) : QFrame(parent)
{

}

AudioContentFrame::AudioContentFrame(QWidget *parent) :
    QFrame(parent)
{
    init_size();
}

void AudioContentFrame::init_size()
{
    int parentWidth = this->parentWidget()->width();
    this->resize(parentWidth, 0);
}

void AudioContentFrame::addFrame(AudioFrame *frame)
{
    frame->resize(this->width(), frame->height());
    frame->setParent(this);
    this->resize(this->width(), this->height() + frame->height());
    int y = 0;
    if(!frames.isEmpty())
    {
        y = frames.last()->y() + frames.last()->height();
    }
    frame->move(0, y);
    frames.append(frame);
}
