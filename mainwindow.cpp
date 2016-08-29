#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>

#include <permissions.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    account(nullptr)
{
    ui->setupUi(this);
//    AudioFrame * last = nullptr;
//    for(int i = 30; i > 0; i--)
//    {
//        AudioFrame * frame = new AudioFrame(ui->audio);
//        if(last != nullptr)
//            frame->move(2, last->height() + last->y() + 2);
//        last = frame;
//        frames.append(frame);
//    }

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
        else
            printAccount();
    }
    this->updater = new UpdateThread(account, this);
    connect(this->updater, &UpdateThread::readFinished, this, &MainWindow::read);
//    QNetworkRequest r;
//    VKApi::Method getAudio = VKApi::Audio::get.createMethod();
//    getAudio["count"] = "20";
//    QUrl url(QString(getAudio.getUri(account->getAccessToken()).c_str()));
//    r.setUrl(url);
//    reply = manager.get(r);
//    connect(reply, &QNetworkReply::readyRead, this, &MainWindow::loadAudios);
//    QLabel * l = new QLabel(account->getAccessToken().c_str());
//    l->show();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete updater;
}

void MainWindow::wheelEvent(QWheelEvent * event)
{
    int delta = event->delta() / 8;
    QWidget * current = nullptr;
    if(ui->tabWidget->currentIndex() == ui->tabWidget->indexOf(ui->audio))
    {
        current = ui->audio;
    }
    if(ui->tabWidget->currentIndex() == ui->tabWidget->indexOf(ui->message))
    {
        current = this->focusWidget();
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

void MainWindow::resizeEvent(QResizeEvent * event)
{

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
        printAccount();
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
    nlohmann::json response = response.parse(bytes.toStdString());
    Log::i("Called \'read\'");
    Log::i(response.dump(1).c_str());
    if(method == VKApi::Messages::getHistory)
    {
        Log::i("Id is founded, updating");
        this->updateMessages(response);
    }
    if(method == VKApi::Audio::get)
    {
        Log::i("Id is founded, updating");
        this->loadAudios(response);
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
    for(QObject * ob : ui->messages->children())
        ob->deleteLater();
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

void MainWindow::printAccount()
{
    if(account)
    {
        ui->accessToken->setText(account->getAccessToken().c_str());
        ui->userId->setText(QString().setNum(account->getUserId()));
    }
}

void MainWindow::loadAudios(nlohmann::json json)
{
    VKObjectFactory f;
    nlohmann::json array = json["response"]["items"];
    AudioFrame * last = nullptr;
    for(QObject * ob : ui->audio->children())
        ob->deleteLater();
    for(auto it : array)
    {
        AudioFrame * frame = new AudioFrame(f.buildAudio(it), ui->audio);
        connect(frame, &AudioFrame::downloadClicked, this, &MainWindow::startDownloadAudio);
        //AudioFrame * frame = new AudioFrame(f.buildObject<Audio>(it), ui->audio);
        frame->show();
        if(last != nullptr)
            frame->move(2, last->height() + last->y() + 2);
        last = frame;
    }
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    if(ui->tabWidget->indexOf(ui->message) == index)
    {
        VKApi::Method mess = VKApi::Messages::getHistory.createMethod();
        mess["user_id"] = std::to_string(account->getUserId());
        mess["count"] = "10";
        this->updater->addMethodToQueue(mess);
    }
    if(ui->tabWidget->indexOf(ui->audio) == index)
    {
        VKApi::Method audios = VKApi::Audio::get.createMethod();
        audios["count"] = "300";
        this->updater->addMethodToQueue(audios);
    }
}

void MainWindow::on_action_triggered()
{
    account.reset();
    showLoginWindow();
}

void MainWindow::on_send_clicked()
{
    if(!ui->text->toPlainText().isEmpty())
    {
        VKApi::Method send = VKApi::Messages::send.createMethod();
        send["user_id"] = std::to_string(account->getUserId());
        send["message"] = ui->text->toPlainText().toStdString().c_str();
        this->updater->addMethodToQueue(send);
        VKApi::Method mess = VKApi::Messages::getHistory.createMethod();
        mess["user_id"] = std::to_string(account->getUserId());
        mess["count"] = "10";
        this->updater->addMethodToQueue(mess);
        ui->text->clear();
    }
}
