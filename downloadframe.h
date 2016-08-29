#ifndef DOWNLOADFRAME_H
#define DOWNLOADFRAME_H

#include <QFrame>

namespace Ui {
class DownloadFrame;
}

class QFile;
class FileDownloader;

class DownloadFrame : public QFrame
{
    Q_OBJECT

    FileDownloader * downloader;
    QFile * file;

public:
    explicit DownloadFrame(QString uri, QString path, QWidget *parent = 0);
    ~DownloadFrame();

public slots:
    void progress(qint64, qint64);
    void finished();

private:
    Ui::DownloadFrame *ui;
};

#endif // DOWNLOADFRAME_H
