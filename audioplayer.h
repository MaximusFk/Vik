#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QWidget>
#include <audio.h>

class QAudioOutput;
class QByteArray;
class QBuffer;
class FileDownloader;
class QMediaPlayer;
class QAudioDecoder;

namespace Ui {
class AudioPlayer;
}

class AudioPlayer : public QWidget
{
    Q_OBJECT

private:
    void init_buffer(std::shared_ptr<Audio>);
    void clear_buffer();
    void init_player();
    void free_player();
    void init_ui(std::shared_ptr<Audio>);

    void update_time_text(int, int);
public:
    explicit AudioPlayer(QWidget *parent = 0);
    ~AudioPlayer();

    void play(std::shared_ptr<Audio>);

    void stop();
    void resume();
    void pause();

private slots:
    void notify();
    void positionChanged(qint64);
    void finishedBuffering();
    void processBuffering(qint64, qint64);

private:
    Ui::AudioPlayer *ui;
    std::shared_ptr<Audio> audio;

    QMediaPlayer * player;
    QByteArray * byteArray;
    QBuffer * buffer;

    QAudioDecoder * decoder;

    ///TODO Заменить на запрос из кэша
    FileDownloader * downloader;

    int currentTick;

    static constexpr int secinms = 1000;
};

#endif // AUDIOPLAYER_H
