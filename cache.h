#ifndef CACHE_H
#define CACHE_H

#include <QObject>
#include <QMap>
#include <QQueue>
#include <QBuffer>
#include <QDir>

class FileDownloader;

class Cache : public QObject
{
    Q_OBJECT
public:
    explicit Cache(QObject *parent = nullptr);
    Cache(QDir * path, QObject * parent = nullptr);

    void enqueue(QString);

    QByteArray * getData(QString);
    bool resetBuffer(QString);
    void cancel();

private:
    void updateStatus();

signals:
    void nextUri(QString);
    void finished(QString, QByteArray *);
    void proccess(qint64, qint64, QString, QByteArray *);

private slots:
    void _finished();
    void _proccess(qint64, qint64);

private:
    FileDownloader * downloader;
    QMap<QString, QByteArray *> cache;
    QQueue<QString> queue;
    QByteArray * current;
    QBuffer * buffer;
    QDir * cacheDir;
};

#endif // CACHE_H
