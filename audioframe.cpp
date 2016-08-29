#include "audioframe.h"
#include "ui_audioframe.h"

AudioFrame::AudioFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::AudioFrame)
{
    ui->setupUi(this);
}

AudioFrame::AudioFrame(std::shared_ptr<Audio> audio, QWidget *parent) :
    QFrame(parent),
    ui(new Ui::AudioFrame),
    audio(audio)
{
    ui->setupUi(this);
    ui->artist_l->setText(audio->getArtist().c_str());
    ui->title_l->setText(audio->getTitle().c_str());
    QString str;
    str += QString().setNum(audio->getDurationMM());
    str += ':';
    if(audio->getDurationSS() < 10)
        str += '0';
    str += QString().setNum(audio->getDurationSS());
    ui->time_l->setText(str);
}

AudioFrame::~AudioFrame()
{
    delete ui;
}

void AudioFrame::on_download_b_clicked()
{
    emit downloadClicked(audio.get());
}

void AudioFrame::on_play_b_clicked()
{
    emit playClicked(audio.get());
}
