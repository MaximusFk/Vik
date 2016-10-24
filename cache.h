#ifndef CACHE_H
#define CACHE_H

#include <QObject>
#include <QMap>
#include <QQueue>
#include <QFile>

class FileDownloader;
class QBuffer;
class QDir;

class Cache : public QObject
{
    Q_OBJECT

    struct Node
    {
        QFile file;
        int id;
        Node(const QFile & file, int id) : file(file.fileName()), id(id){}
        Node(const Node & o) : file(o.file.fileName()), id(o.id){}
        Node(int id) : id(id){}
        bool operator ==(const Node & other)
        {
            return this->id == other.id;
        }
    };

    struct CacheNode
    {
        int id;
        QString uri;

        CacheNode(int id) : id(id){}
        CacheNode(int id, QString uri) : id(id), uri(uri){}

        bool operator <(const CacheNode & other)
        {
            return this->id < other.id;
        }
        friend bool operator <(const CacheNode & arg1, const CacheNode & arg2)
        {
            return arg1.id < arg2.id;
        }

        bool operator ==(const CacheNode & other)
        {
            return this->id == other.id;
        }
    };

public:
    explicit Cache(QObject *parent = nullptr);
    Cache(QDir * path, QObject * parent = nullptr);

    void enqueue(QString, int, bool cacheOnDisk = false);
    bool resetBuffer(QString);
    void cancel();

    bool cachedOnDisk(int) const;

private:
    void updateStatus();
    bool saveToDisk(int, QByteArray *);
    bool loadFromDisk(int);

    Node getNodeById(int) const;

signals:
    void nextUri(QString);
    void finished(QString, QByteArray *);
    void proccess(qint64, qint64, QString, QByteArray *);

private slots:
    void _finished();
    void _proccess(qint64, qint64);

private:
    FileDownloader * downloader;
    QList<Node> diskCaches;
    QMap<CacheNode, QByteArray *> cache;
    QQueue<CacheNode> queue;
    CacheNode currentNode;
    QByteArray * current;
    QBuffer * buffer;
    QDir * cacheDir;
    bool saveToDiskf;
};

#endif // CACHE_H
