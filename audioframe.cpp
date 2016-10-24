#include "audioframe.h"
#include "ui_audioframe.h"

#include "audioplayer.h"
#include "updatethread.h"

#include <QResizeEvent>

AudioFrame::AudioFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::AudioFrame),
    player(nullptr)
{
    ui->setupUi(this);
}

AudioFrame::AudioFrame(std::shared_ptr<Audio> audio, QWidget *parent) :
    QFrame(parent),
    ui(new Ui::AudioFrame),
    audio(audio),
    player(nullptr)
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
    init_frame_size();
}

AudioFrame::~AudioFrame()
{
    delete ui;
}

void AudioFrame::setAudioPlayer(AudioPlayer * player)
{
    this->player = player;
}

void AudioFrame::init_frame_size()
{
    QSize parentSize = this->parentWidget()->size();
    int parentWidth = parentSize.width();
    this->resize(parentWidth, this->height());
    int dwbXPos = this->width() - ui->download_b->width();
    int tlXPos = dwbXPos - ui->time_l->width() - 2;
    int artistWidth = tlXPos - ui->artist_l->x();
    ui->download_b->move(dwbXPos, ui->download_b->y());
    ui->time_l->move(tlXPos, ui->time_l->y());
    ui->artist_l->resize(artistWidth, ui->artist_l->height());
    ui->title_l->resize(artistWidth, ui->title_l->height());
}

void AudioFrame::resizeEvent(QResizeEvent * rs)
{
    int newWidth = rs->size().width();
    int dwbXPos = newWidth - ui->download_b->width();
    int tlXPos = dwbXPos - ui->time_l->width() - 2;
    int artistWidth = tlXPos - ui->artist_l->x();
    ui->download_b->move(dwbXPos, ui->download_b->y());
    ui->time_l->move(tlXPos, ui->time_l->y());
    ui->artist_l->resize(artistWidth, ui->artist_l->height());
    ui->title_l->resize(artistWidth, ui->title_l->height());
}

void AudioFrame::on_download_b_clicked()
{
    emit downloadClicked(audio.get());
}

void AudioFrame::on_play_b_clicked()
{
    emit playClicked(audio.get());
    if(player != nullptr)
        player->play(audio);
}
