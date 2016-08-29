#ifndef UPDATETHREAD_H
#define UPDATETHREAD_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QQueue>
#include <QList>
#include <QTimer>
#include <QIODevice>

#include <vkapicpplibrary.h>
#include <account.h>
using VKApi::Method;

class UpdateThread : public QObject
{
    Q_OBJECT

    struct MethodDelay
    {
        Method method;
        int cdelay;
        int currentDelay;
    };

    std::shared_ptr<Account> account;

    QNetworkAccessManager * manager;
    QNetworkReply * reply;
    QQueue<Method> methodQueue;
    QList<MethodDelay> sheulder;
    Method current;
    QByteArray * currentBlock;

    QTimer * timer;
protected:
    void updateReplyStatus();

public:
    explicit UpdateThread(std::shared_ptr<Account> account, QObject *parent = 0);
    virtual ~UpdateThread();

    void addMethodToQueue(const Method &);
    void addMethodToDelay(const Method &, int);

signals:
    void readFinished(const Method &, const QByteArray &);
    void methodRequestStart(const Method &);

public slots:

private slots:
    void readBlock();
    void finished();
    void update();
};

class FileDownloader : public QObject
{

    Q_OBJECT

    QString uri;
    QNetworkAccessManager * manager;
    QNetworkReply * reply;
    QIODevice * buffer;

public:
    explicit FileDownloader(QString uri, QIODevice * device, QObject * parent = 0);
    virtual ~FileDownloader();

    bool isRunning() const;
    QString getUri() const;

private slots:
    void readBlock();
    void finished();
    void download(qint64, qint64);

signals:
    void finishedDownload();
    void progressDownload(qint64, qint64);

public:

    QIODevice * getDevice();
};

#endif // UPDATETHREAD_H
