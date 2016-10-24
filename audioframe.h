#ifndef AUDIOFRAME_H
#define AUDIOFRAME_H

#include <QFrame>
#include <audio.h>

namespace Ui {
class AudioFrame;
}

class AudioPlayer;
class FileDownloader;

class AudioFrame : public QFrame
{
    Q_OBJECT

public:
    explicit AudioFrame(QWidget *parent = 0);
    AudioFrame(std::shared_ptr<Audio> audio, QWidget *parent = 0);
    ~AudioFrame();

    void setAudioPlayer(AudioPlayer *);

private:
    void init_frame_size();

protected:
    virtual void resizeEvent(QResizeEvent *) override;

private slots:
    void on_download_b_clicked();

    void on_play_b_clicked();

signals:
    void downloadClicked(Audio *);
    void playClicked(Audio *);

private:
    Ui::AudioFrame *ui;
    std::shared_ptr<Audio> audio;
    AudioPlayer * player;
};

#endif // AUDIOFRAME_H
