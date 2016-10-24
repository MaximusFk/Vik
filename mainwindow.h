#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>

#include <QWheelEvent>
#include <QResizeEvent>
#include <account.h>

#include <vkobjectfactory.h>
#include <vkapicpplibrary.h>

#include "updatethread.h"

namespace Ui {
class MainWindow;
}

class LoginWidget;
class Cache;
class AudioPlayer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    VKObjectFactory * factory;

    std::shared_ptr<Account> account;
    LoginWidget * login;

    UpdateThread * updater;
    UpdateThread * update_loop;
    Cache * mainCache;
    AudioPlayer * audioPlayer;

    struct Page
    {
        int count = 0;
        int offset = 0;
    };
    Page audio_page;
    Page message_page;

    enum State
    {
        IDLE = -1,
        DIALOG_LOADING,
        MESSAGE_LOADING,
        AUDIO_LOADING
    }
    current_state;

    QList<std::shared_ptr<IVKObject>> object_buffer;

    std::shared_ptr<User> user;

private:
    void saveAccountToDisk();
    bool loadAccountFromDisk();
    void showLoginWindow();

    void updateMessages(nlohmann::json);
    void loadAudios(nlohmann::json json);

    void changeState(State);

    void constructFrameFromBuffer(State);

    void clearAllChildren(QWidget *);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    void wheelEvent(QWheelEvent *) override;

public slots:
    void urlChanged(const QUrl &);
    void read(const Method &, const QByteArray &);
    void showDiloagMessages(User *);

    void startDownloadAudio(Audio *);

private slots:
    void on_tabWidget_currentChanged(int index);

    void on_action_triggered();

    void on_send_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
