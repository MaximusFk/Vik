#include "downloadframe.h"
#include "ui_downloadframe.h"
#include <QFile>
#include "updatethread.h"

DownloadFrame::DownloadFrame(QString uri, QString path, QWidget *parent) :
    QFrame(parent),
    ui(new Ui::DownloadFrame)
{
    ui->setupUi(this);
    file = new QFile(path, this);
    downloader = new FileDownloader(uri, file, this);
    ui->uri->setText(uri);
    ui->file->setText(path);
    connect(downloader, &FileDownloader::finishedDownload, this, &DownloadFrame::finished);
    connect(downloader, &FileDownloader::progressDownload, this, &DownloadFrame::progress);
}

DownloadFrame::~DownloadFrame()
{
    delete ui;
    file->deleteLater();
    downloader->deleteLater();
}

void DownloadFrame::progress(qint64 len, qint64 pos)
{
    ui->progress->setMaximum(pos);
    ui->progress->setValue(len);
}

void DownloadFrame::finished()
{
    if(file)
    {
        file->flush();
        file->close();
    }
    ui->progress->setVisible(false);
}
