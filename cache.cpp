#include "cache.h"
#include "updatethread.h"

#include <QFile>

Cache::Cache(QObject *parent) :
    QObject(parent),
    downloader(nullptr),
    current(nullptr),
    buffer(nullptr),
    cacheDir(nullptr)
{
}

Cache::Cache(QDir *path, QObject *parent) :
    Cache(parent)
{
    cacheDir = path;
}

void Cache::enqueue(QString uri)
{
    if(cache.contains(uri))
    {
        emit finished(uri, cache[uri]);
    }
    else
    {
        queue.enqueue(uri);
        updateStatus();
    }
}

QByteArray * Cache::getData(QString uri)
{
    if(cache.contains(uri))
        return cache[uri];
    else
    {
        enqueue(uri);
        return nullptr;
    }
}

void Cache::cancel()
{
    if(downloader != nullptr)
    {
        delete current;
        current = nullptr;
        disconnect(downloader, &FileDownloader::progressDownload, this, &Cache::_proccess);
        disconnect(downloader, &FileDownloader::finishedDownload, this, &Cache::_finished);
        downloader->deleteLater();
        downloader = nullptr;
        buffer->deleteLater();
        buffer = nullptr;
        downloader->deleteLater();
        downloader = nullptr;
    }
}

void Cache::updateStatus()
{
    if(downloader != nullptr)
    {
        if(downloader->isRunning())
            return;
        cache.insert(downloader->getUri(), current);
        current = nullptr;
        disconnect(downloader, &FileDownloader::progressDownload, this, &Cache::_proccess);
        disconnect(downloader, &FileDownloader::finishedDownload, this, &Cache::_finished);
        downloader->deleteLater();
        downloader = nullptr;
        buffer->deleteLater();
        buffer = nullptr;
    }
    if(!queue.isEmpty())
    {
        emit nextUri(queue.head());
        current = new QByteArray();
        buffer = new QBuffer(current, this);
        downloader = new FileDownloader(queue.dequeue(), buffer, this);
        connect(downloader, &FileDownloader::progressDownload, this, &Cache::_proccess);
        connect(downloader, &FileDownloader::finishedDownload, this, &Cache::_finished);
    }
}

void Cache::_finished()
{
    emit finished(downloader->getUri(), current);
    updateStatus();
}

void Cache::_proccess(qint64 len, qint64 size)
{
    emit proccess(len, size, downloader->getUri(), current);
}
