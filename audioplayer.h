#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QWidget>

namespace Ui {
class AudioPlayer;
}

class AudioPlayer : public QWidget
{
    Q_OBJECT

public:
    explicit AudioPlayer(QWidget *parent = 0);
    ~AudioPlayer();

private:
    Ui::AudioPlayer *ui;
};

#endif // AUDIOPLAYER_H
