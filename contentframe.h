#ifndef CONTENTFRAME_H
#define CONTENTFRAME_H

#include <QFrame>

class AudioFrame;

class ContentFrame : public QFrame
{
    Q_OBJECT
public:
    explicit ContentFrame(QWidget *parent = 0);
    virtual void addFrame(QFrame *) = 0;

signals:

public slots:
};

class AudioContentFrame : public QFrame
{
    Q_OBJECT

private:
    void init_size();
public:
    explicit AudioContentFrame(QWidget * parent = nullptr);
    void addFrame(AudioFrame *);

private:
    QList<QFrame *> frames;
};

#endif // CONTENTFRAME_H
