#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QWheelEvent>
#include <QResizeEvent>

#include "audioframe.h"
#include "messageframe.h"
#include "loginwidget.h"
#include <account.h>

#include <vkobjectfactory.h>
#include <vkapicpplibrary.h>

#include "updatethread.h"

namespace Ui {
class MainWindow;
}
static constexpr char * dasha = "190529637";

class MainWindow : public QMainWindow
{
    Q_OBJECT
    std::shared_ptr<Account> account;
    LoginWidget *login;

    UpdateThread * updater;

    struct Page
    {
        int count = 0;
        int offset = 0;
    };
    Page audio_page;
    Page message_page;

private:
    void saveAccountToDisk();
    bool loadAccountFromDisk();
    void printAccount();
    void showLoginWindow();

    void updateMessages(nlohmann::json);
    void loadAudios(nlohmann::json json);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    void wheelEvent(QWheelEvent *) override;
    void resizeEvent(QResizeEvent *) override;

public slots:
    void urlChanged(const QUrl &);
    void read(const Method &, const QByteArray &);

    void startDownloadAudio(Audio *);

private slots:
    void on_tabWidget_currentChanged(int index);

    void on_action_triggered();

    void on_send_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
