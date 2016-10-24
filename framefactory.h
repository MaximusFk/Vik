#ifndef FRAMEFACTORY_H
#define FRAMEFACTORY_H
#include <QObject>

class UpdateThread;


class FrameFactory
{
public:
    explicit FrameFactory();
};

class DialogFrameFactory : QObject {
    UpdateThread * updater;
public:
    DialogFrameFactory(QObject * parent);
};

#endif // FRAMEFACTORY_H
