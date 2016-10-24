#include "cache.h"
#include "updatethread.h"

#include <QBuffer>
#include <QDir>

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

Cache::Cache(QObject *parent) :
    QObject(parent),
    downloader(nullptr),
    currentNode(0),
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

void Cache::enqueue(QString uri, int id, bool cacheOnDisk)
{
    if(cache.contains(id))
    {
        emit finished(uri, cache[id]);
    }
    else
    {
        queue.enqueue({id, uri});
        updateStatus();
    }
    saveToDiskf = cacheOnDisk;
}

bool Cache::cachedOnDisk(int id) const
{
    return diskCaches.indexOf(id) >= 0;
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
    }
}

void Cache::updateStatus()
{
    if(downloader != nullptr)
    {
        if(downloader->isRunning())
            return;
        cache.insert(currentNode, current);
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
        emit nextUri(queue.head().uri);
        current = new QByteArray();
        buffer = new QBuffer(current, this);
        currentNode = queue.dequeue();
        downloader = new FileDownloader(currentNode.uri, buffer, this);
        connect(downloader, &FileDownloader::progressDownload, this, &Cache::_proccess);
        connect(downloader, &FileDownloader::finishedDownload, this, &Cache::_finished);
    }
}

bool Cache::saveToDisk(int id, QByteArray * buffer)
{
    QFile jsonFile(this->cacheDir->absoluteFilePath("cache.json"));
    jsonFile.open(QIODevice::ReadWrite);
    QJsonDocument json = jsonFile.exists()
            ? QJsonDocument::fromBinaryData(jsonFile.readAll())
            : QJsonDocument(QJsonArray());
    Node node = {QFile(this->cacheDir->absoluteFilePath(QString::number(id))), id};
    QJsonObject jnode;
    jnode.insert("file", node.file.fileName());
    jnode.insert("id", id);
    json.array().append(jnode);
    jsonFile.write(json.toBinaryData());
    jsonFile.flush();
    jsonFile.close();
    node.file.open(QIODevice::WriteOnly);
    node.file.write(*buffer);
    node.file.flush();
    node.file.close();
    return true;
}

bool Cache::loadFromDisk(int id)
{
    Node node = getNodeById(id);
    if(node.id != 0 && node.file.exists())
    {
        node.file.open(QIODevice::ReadOnly);
        QByteArray * array = new QByteArray(node.file.readAll());
        CacheNode cnode = node.id;
        cache.insert(cnode, array);
        node.file.close();
        return true;
    }
    return false;
}

Cache::Node Cache::getNodeById(int id) const
{
    int i = diskCaches.indexOf(id);
    return i >= 0 ? diskCaches[i] : Node(QFile(), 0);
}

void Cache::_finished()
{
    if(saveToDiskf)
    {
        saveToDisk(currentNode.id, current);
    }
    emit finished(currentNode.uri, current);
    updateStatus();
}

void Cache::_proccess(qint64 len, qint64 size)
{
    emit proccess(len, size, downloader->getUri(), current);
}
