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
    file->open(QIODevice::ReadWrite);
    downloader = new FileDownloader(uri, file, this);
    ui->uri->setText(uri.split('?').at(0));
    ui->file->setText(path);
    connect(downloader, &FileDownloader::finishedDownload, this, &DownloadFrame::finished);
    connect(downloader, &FileDownloader::progressDownload, this, &DownloadFrame::progress);
    connect(ui->cancel, &QPushButton::clicked, this, &DownloadFrame::cancel);
}

DownloadFrame::~DownloadFrame()
{
    delete ui;
    file->deleteLater();
    downloader->deleteLater();
}

void DownloadFrame::cancel()
{
    if(downloader != nullptr)
    {
        file->close();
        downloader->cancel();
        downloader->deleteLater();
        downloader = nullptr;
    }
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
