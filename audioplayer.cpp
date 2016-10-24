#include "audioplayer.h"
#include "ui_audioplayer.h"

#include <QMediaPlayer>
#include <QAudioDeviceInfo>
#include <QBuffer>
#include "updatethread.h"

void AudioPlayer::init_buffer(std::shared_ptr<Audio> description)
{
    clear_buffer();
    byteArray = new QByteArray;
    buffer = new QBuffer(byteArray, this);
    buffer->open(QIODevice::ReadWrite);
    downloader = new FileDownloader(this);
    connect(downloader, &FileDownloader::finishedDownload, this, &AudioPlayer::finishedBuffering);
    connect(downloader, &FileDownloader::progressDownload, this, &AudioPlayer::processBuffering);
    downloader->startDownload(QString::fromStdString(description->getURI()), buffer);
}

void AudioPlayer::clear_buffer()
{
    if(downloader != nullptr)
    {
        downloader->cancel();
        disconnect(downloader, &FileDownloader::finishedDownload, this, &AudioPlayer::finishedBuffering);
        downloader->deleteLater();
        downloader = nullptr;
    }
    if(byteArray != nullptr)
    {
        delete byteArray;
        byteArray = nullptr;
    }
    if(buffer != nullptr)
    {
        buffer->deleteLater();
        buffer = nullptr;
    }
}

void AudioPlayer::init_player()
{
    free_player();
    player = new QMediaPlayer(this);
    //player->setMedia(QMediaContent(), buffer);
    //player->setMedia(downloader->getQUrl());
    player->setVolume(100);
    //player->play();
    connect(player, &QMediaPlayer::positionChanged, this, &AudioPlayer::positionChanged);
}

void AudioPlayer::free_player()
{
    if(player != nullptr)
    {
        player->stop();
        disconnect(player, &QMediaPlayer::positionChanged, this, &AudioPlayer::positionChanged);
        player->deleteLater();
        player = nullptr;
    }
    currentTick = 0;
}

void AudioPlayer::init_ui(std::shared_ptr<Audio> audio)
{
    ui->artist->setText(QString::fromStdString(audio->getArtist()));
    ui->title->setText(QString::fromStdString(audio->getTitle()));
    ui->position->setMaximum(audio->getDuration());
    update_time_text(0, audio->getDuration());
}

void AudioPlayer::update_time_text(int current, int max)
{
    int current_minutes, current_seconds, max_minutes, max_seconds;
    current_minutes = compute_minutes(current, current_seconds);
    max_minutes = compute_minutes(max, max_seconds);
    QString string;
    string += QString::number(current_minutes);
    string += ':';
    string += current_seconds ?
                current_seconds < 10 ?
                    "0" + QString::number(current_seconds) :
                QString::number(current_seconds) :
                "00";
    string += '/';
    string += QString::number(max_minutes);
    string += ':';
    string += max_seconds ?
                max_seconds < 10 ?
                    "0" + QString::number(max_seconds) :
                QString::number(max_seconds) :
                "00";
    ui->time->setText(string);
}

AudioPlayer::AudioPlayer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AudioPlayer),
    player(nullptr),
    byteArray(nullptr),
    buffer(nullptr),
    downloader(nullptr),
    currentTick(0)
{
    ui->setupUi(this);
    ui->position->setMinimum(0);
}

AudioPlayer::~AudioPlayer()
{
    delete ui;
    free_player();
    clear_buffer();
}

void AudioPlayer::play(std::shared_ptr<Audio> audioNode)
{
    free_player();
    audio = audioNode;
    init_ui(audio);
    init_buffer(audio);
    init_player();
    this->showNormal();
}

void AudioPlayer::pause()
{
    if(player != nullptr)
        player->pause();
}

void AudioPlayer::resume()
{
    if(player != nullptr)
        player->play();
}

void AudioPlayer::notify()
{
    qDebug() << "notify state: " << player->state();
    ++currentTick;
    qDebug() << "time: " << currentTick;
    update_time_text(currentTick, audio->getDuration());
}

void AudioPlayer::positionChanged(qint64)
{

}

void AudioPlayer::finishedBuffering()
{
    //init_player();
    ///TODO Определить запись буффера на диск
}

void AudioPlayer::processBuffering(qint64 bytes, qint64 size)
{
    qDebug() << "buffered: " << bytes << " bytes of " << size;
}
