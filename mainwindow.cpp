#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QDir>

#include <permissions.h>

#include <user.h>
#include "cache.h"

#include "dialogframe.h"
#include "audioframe.h"
#include "messageframe.h"
#include "loginwidget.h"
#include "audioplayer.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    factory(new VKObjectFactory()),
    account(nullptr),
    mainCache(new Cache(new QDir("./"), this)),
    audioPlayer(new AudioPlayer),
    current_state(IDLE),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Получение аккаунта пользователя
    if(!account) {
        if(!loadAccountFromDisk())
        {
            VKApi::Method l = VKApi::Auth::authorize.createMethod();
            l["client_id"] = "5537666";
            l["redirect_uri"] = VKApi::defaultRedirect;
            l["display"] = "mobile";
            l["scope"] = "audio";
            l["response_type"] = "token";
            login = new LoginWidget(l.getUri().c_str());
            connect(login, &LoginWidget::urlChanged, this, &MainWindow::urlChanged);
            login->show();
        }
    }
    this->updater = new UpdateThread(account, this);
    connect(this->updater, &UpdateThread::readFinished, this, &MainWindow::read);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete updater;
    delete mainCache;
    if(audioPlayer->isVisible())
        audioPlayer->close();
    audioPlayer->deleteLater();
}

void MainWindow::wheelEvent(QWheelEvent * event)
{
    int delta = event->delta() / 8;
    QWidget * current = nullptr;
    if(ui->tabWidget->currentIndex() == ui->tabWidget->indexOf(ui->audio))
    {
        current = ui->audio_list;
    }
    if(ui->tabWidget->currentIndex() == ui->tabWidget->indexOf(ui->message))
    {
        current = ui->messages;
    }

    if(!current || current->children().isEmpty())
        return;

    QWidget * last = dynamic_cast<QWidget *>(current->children().last());
    QWidget * first = dynamic_cast<QWidget *>(current->children().first());

    if(first && last)
    {
        if((delta < 0 && last->y() + last->height() > current->height()) ||
            (delta > 0 && first->y() < 0))
            for(QObject * object : current->children())
            {
                QWidget * widget = dynamic_cast<QWidget *>(object);
                if(widget)
                {
                    int newYpos = widget->y() + delta;
                    widget->move(widget->x(), newYpos);
                }
            }
    }
    event->accept();
}

void MainWindow::urlChanged(const QUrl & url)
{
    if(url.url().indexOf("access_token") >= 0){
        try{
            Account * a = new Account(url.url().toStdString());
            account = std::shared_ptr<Account>(a);
            saveAccountToDisk();
        }
        catch(exception v) {
            // ...
        }
        login->close();
    }
}

void MainWindow::showLoginWindow()
{
    if(!account) {
        VKApi::Method l = VKApi::Auth::authorize.createMethod();
        l["client_id"] = "5537666";
        l["redirect_uri"] = VKApi::defaultRedirect;
        l["display"] = "mobile";
        l["scope"] = std::to_string(VKApiPermissions::audio | VKApiPermissions::messages | VKApiPermissions::offline);
        l["response_type"] = "token";
        login = new LoginWidget(l.getUri().c_str());
        connect(login, &LoginWidget::urlChanged, this, &MainWindow::urlChanged);
        login->show();
    }
}

#include <fstream>
#include "Log.h"
#include <QFileDialog>
#include <QFile>
#include "downloadframe.h"

void MainWindow::read(const Method & method, const QByteArray & bytes)
{
    using namespace VKApi;
    nlohmann::json response = response.parse(bytes.toStdString());
    Log::i("Called \'read\'");
    Log::i(response.dump(1).c_str());
    if(method == Messages::getHistory)
    {
        Log::i("Id is founded, updating");
        this->updateMessages(response);
    }
    if(method == Audio::get)
    {
        Log::i("Id is founded, updating");
        this->loadAudios(response);
    }
    if(method == Messages::getDialogs && current_state == DIALOG_LOADING)
    {
        nlohmann::json array = response["response"]["items"];
        Method m = Users::get;
        for(auto it : array)
        {
            std::shared_ptr<Message> message = factory->buildMessage(it["message"]);
            object_buffer.push_back(message);
            m.add("user_ids", message->getUserId());
        }
        updater->addMethodToQueue(m);
    }
    if(method == Users::get && current_state == DIALOG_LOADING)
    {
        nlohmann::json array = response["response"];
        for(auto it : array)
        {
            object_buffer.push_back(factory->buildUser(it));
        }
        constructFrameFromBuffer(current_state);
        current_state = IDLE;
        object_buffer.clear();
    }
}

void MainWindow::startDownloadAudio(Audio * audio)
{
    QString name;
    name += QString::fromStdString(audio->getArtist());
    name += " - ";
    name += QString::fromStdString(audio->getTitle());
    name += ".mp3";
    QString dialog = QFileDialog::getSaveFileName(this, tr("Сохранить"), name);
    if(!dialog.isEmpty())
    {
        DownloadFrame * d = new DownloadFrame(QString::fromStdString(audio->getURI()), dialog, ui->downloads);
        d->show();
        ui->tabWidget->setCurrentIndex(3);
    }
}

void MainWindow::updateMessages(nlohmann::json json)
{
    VKObjectFactory f;
    nlohmann::json array = json["response"]["items"];
    MessageFrame * last = nullptr;
    clearAllChildren(ui->messages);
    for(auto it : array)
    {
        MessageFrame * frame = new MessageFrame(f.buildMessage(it), ui->messages);
        frame->show();
        int startX = frame->isOut() ?
                    ui->messages->width() - frame->width() - 2 :
                    2;
        int startY = last != nullptr ?
                    last->y() - frame->height() - 2 :
                    ui->messages->height() - frame->height() - 2;
        frame->move(startX, startY);
        last = frame;
    }
}

void MainWindow::saveAccountToDisk()
{
    std::ofstream file("account.json", std::ofstream::out);
    VKObjectFactory f;
    file << f.toJSON(account);
    file.flush();
    file.close();
}

bool MainWindow::loadAccountFromDisk()
{
    nlohmann::json json;
    std::ifstream file("account.json", std::ifstream::in);
    if(file.is_open()) {
        file >> json;
        VKObjectFactory f;
        account = f.buildAccount(json);
        file.close();
        return true;
    }
    return false;
}

void MainWindow::loadAudios(nlohmann::json json)
{
    VKObjectFactory f;
    nlohmann::json array = json["response"]["items"];
    AudioFrame * last = nullptr;
    clearAllChildren(ui->audio_list);
    for(auto it : array)
    {
        AudioFrame * frame = new AudioFrame(f.buildAudio(it), ui->audio_list);
        frame->setAudioPlayer(audioPlayer);
        connect(frame, &AudioFrame::downloadClicked, this, &MainWindow::startDownloadAudio);
        frame->show();
        if(last != nullptr)
            frame->move(2, last->height() + last->y() + 2);
        last = frame;
    }
}

void MainWindow::showDiloagMessages(User * user)
{
    this->updater->addMethodToQueue(VKApi::Messages::getHistory.createMethod()
                                    .add("user_id", user->getId())
                                    .add("count", 20)
                                    );
    this->user = std::shared_ptr<User>(user);
}

void MainWindow::changeState(MainWindow::State state)
{
    current_state = state;
    Log::i((std::string("set new status: ") + std::to_string(state)));
}

///TODO: Плохо спроектировано!!!! Переделать!!!
void MainWindow::constructFrameFromBuffer(MainWindow::State state)
{
    Log::i("called frame constructor");
    switch(state)
    {
    case DIALOG_LOADING:
        int y = 0;
        clearAllChildren(ui->dialogs);
        for(std::shared_ptr<IVKObject> ob : object_buffer)
        {
            std::shared_ptr<Message> message = std::dynamic_pointer_cast<Message>(ob);
            if(message)
            {
                for(std::shared_ptr<IVKObject> vv : object_buffer)
                {
                    std::shared_ptr<User> us = std::dynamic_pointer_cast<User>(vv);
                    if(us && us->getId() == message->getUserId())
                    {
                        DialogFrame * frame = new DialogFrame(us, message, ui->dialogs);
                        connect(frame, &DialogFrame::click, this, &MainWindow::showDiloagMessages);
                        frame->move(0, y);
                        y += frame->height();
                        frame->show();
                        break;
                    }
                }
            }
        }
        break;
    }
}

void MainWindow::clearAllChildren(QWidget * widget)
{
    for(QObject * child : widget->children())
        child->deleteLater();
}

// -------------------------------Кнопки интерфейса-------------------------------------

void MainWindow::on_tabWidget_currentChanged(int index)
{
    if(ui->tabWidget->indexOf(ui->message) == index)
    {
        this->updater->addMethodToQueue
                (
                    VKApi::Messages::getDialogs.createMethod()
                );
        changeState(DIALOG_LOADING);
    }
    if(ui->tabWidget->indexOf(ui->audio) == index)
    {
        VKApi::Method audios = VKApi::Audio::get.createMethod();
        audios["count"] = "300";
        this->updater->addMethodToQueue(audios);
    }
}
//Сброс аккаунта
void MainWindow::on_action_triggered()
{
    account.reset();
    showLoginWindow();
}
//Отправить сообщение
void MainWindow::on_send_clicked()
{
    if(!ui->text->toPlainText().isEmpty() && user)
    {
        VKApi::Method send = VKApi::Messages::send.createMethod();
        send["user_id"] = std::to_string(user->getId());
        send["message"] = ui->text->toPlainText().toStdString().c_str();
        this->updater->addMethodToQueue(send);
        VKApi::Method mess = VKApi::Messages::getHistory.createMethod();
        mess["user_id"] = std::to_string(user->getId());
        mess["count"] = "10";
        this->updater->addMethodToQueue(mess);
        ui->text->clear();
    }
}
