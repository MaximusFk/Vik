#include "updatethread.h"
#include <QNetworkRequest>
#include <QMutex>

#include "Log.h"

UpdateThread::UpdateThread(std::shared_ptr<Account> account, QObject *parent) :
    QObject(parent)
{
    this->account = account;
    currentBlock = nullptr;
    reply = nullptr;
    manager = new QNetworkAccessManager(this);
    timer = new QTimer(this);
    timer->setInterval(500);
    connect(timer, &QTimer::timeout, this, &UpdateThread::update);
    timer->start();
    Log::i("Object created");
}

UpdateThread::~UpdateThread()
{
    manager->deleteLater();
    reply->deleteLater();
    timer->stop();
    timer->deleteLater();
    if(currentBlock != nullptr)
        delete currentBlock;
}

void UpdateThread::addMethodToQueue(const Method & method)
{
    Log::i("Pre add to queue");
    //QMutex mutex;
    //mutex.lock();
    methodQueue.enqueue(method);
    Log::i("Method added to queue");
    Log::i(method.getMethodName().c_str());
    //mutex.unlock();
    updateReplyStatus();
}

void UpdateThread::addMethodToDelay(const Method & method, int delay)
{
    Log::i("Pre add to delay");
    //QMutex mutex;
    //mutex.lock();
    MethodDelay md = {method, delay, delay};
    this->sheulder.append(md);
    Log::i("Method added to delay");
    Log::i(md.method.getMethodName().c_str());
    //mutex.unlock();
}

void UpdateThread::updateReplyStatus()
{
    Log::i("Updating reply status");
    //QMutex mutex;
    if(reply == nullptr && !methodQueue.isEmpty())
    {
        //mutex.lock();
        Log::i("Getting method from queue");
        current = methodQueue.dequeue();
        QNetworkRequest request((QUrl(current.getUri(account->getAccessToken()).c_str())));
        reply = manager->get(request);
        Log::i("Reply created");
        //mutex.unlock();

        connect(reply, &QNetworkReply::readyRead, this, &UpdateThread::readBlock);
        connect(reply, &QNetworkReply::finished, this, &UpdateThread::finished);
        emit methodRequestStart(current);
    }
}

void UpdateThread::update()
{
    for(MethodDelay d : this->sheulder)
    {
        d.currentDelay -= timer->interval();
        if(d.currentDelay <= 0)
        {
            d.currentDelay = d.cdelay;
            addMethodToQueue(d.method);
            updateReplyStatus();
        }
    }
}

void UpdateThread::readBlock()
{
    Log::i("Reading data block");
    if(currentBlock == nullptr)
        currentBlock = new QByteArray();
    currentBlock->append(reply->readAll());
}

void UpdateThread::finished()
{
    if(currentBlock != nullptr)
    {
        Log::i("Emit signal \'readFinished\'");
        emit readFinished(current, *currentBlock);
        delete currentBlock;
        currentBlock = nullptr;
    }
    reply->deleteLater();
    reply = nullptr;
    Log::i("Reply finished");
    updateReplyStatus();
}

FileDownloader::FileDownloader(QObject *parent) :
    QObject(parent),
    uri(""),
    manager(new QNetworkAccessManager(this)),
    reply(nullptr),
    buffer(nullptr)
{
}

FileDownloader::FileDownloader(QString uri, QIODevice *device, QObject *parent) :
    QObject(parent),
    uri(uri)
{
    manager = new QNetworkAccessManager(this);
    buffer = device;
    QNetworkRequest request((QUrl(uri)));
    reply = manager->get(request);

    connect(reply, &QNetworkReply::readyRead, this, &FileDownloader::readBlock);
    connect(reply, &QNetworkReply::finished, this, &FileDownloader::finished);
    connect(reply, &QNetworkReply::downloadProgress, this, &FileDownloader::download);
}

FileDownloader::~FileDownloader()
{
    if(reply != nullptr)
    {
        disconnect(reply, &QNetworkReply::readyRead, this, &FileDownloader::readBlock);
        disconnect(reply, &QNetworkReply::finished, this, &FileDownloader::finished);
        disconnect(reply, &QNetworkReply::downloadProgress, this, &FileDownloader::download);
        reply->deleteLater();
    }
    manager->deleteLater();
}

void FileDownloader::startDownload(QString uri, QIODevice * device)
{
    buffer = device;
    QNetworkRequest request((QUrl(uri)));
    this->uri = uri;
    reply = manager->get(request);

    connect(reply, &QNetworkReply::readyRead, this, &FileDownloader::readBlock);
    connect(reply, &QNetworkReply::finished, this, &FileDownloader::finished);
    connect(reply, &QNetworkReply::downloadProgress, this, &FileDownloader::download);
}

void FileDownloader::cancel()
{
    if(reply != nullptr)
    {
        disconnect(reply, &QNetworkReply::readyRead, this, &FileDownloader::readBlock);
        disconnect(reply, &QNetworkReply::finished, this, &FileDownloader::finished);
        disconnect(reply, &QNetworkReply::downloadProgress, this, &FileDownloader::download);
        reply->deleteLater();
    }
    reply = nullptr;
    buffer = nullptr;
    uri = "";
}

bool FileDownloader::isRunning() const
{
    return reply != nullptr ? reply->isRunning() : false;
}

QString FileDownloader::getUri() const
{
    return uri;
}

QUrl FileDownloader::getQUrl() const
{
    return QUrl(getUri());
}

QIODevice * FileDownloader::getDevice()
{
    return buffer;
}

void FileDownloader::readBlock()
{
    buffer->write(reply->readAll());
}

void FileDownloader::finished()
{
    emit finishedDownload();
}

void FileDownloader::download(qint64 len, qint64 total)
{
    emit progressDownload(len, total);
}
