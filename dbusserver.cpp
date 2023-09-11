#include "dbusserver.h"
#include "textedit.h"

#include <memory.h>
#include <QDBusMessage>
#include <QThread>


DbusServer::DbusServer(TextEdit* te, QString session_name, QObject *parent)
    : QObject(parent), te_(te)
{
    QDBusConnection session = QDBusConnection::sessionBus();

    if (!session.isConnected()) {
        qWarning("Cannot connect to the D-Bus session bus.\n"
                 "Please check your system settings and try again.\n");
        return;
    }

    if( ! session_name.isEmpty())
        dbus_server_name_ += "." + session_name;

    //  Регистрация сервиса
    if (session.registerService(dbus_server_name_))
    {
        //  Запущен первый экземпляр в сессии
        have_actual_text = true;
        registrate_new_server = true;
    }
    else
    {
        QDBusMessage msg = QDBusMessage::createSignal("/", dbus_server_name_, "textEditStart");
        session.send(msg);
    }

    //  Присоединение сигнала
    session.connect(QString(), QString(),
                    dbus_server_name_, "getActualText",
                    this,
                    SLOT(checkActualText(long long)));

    session.connect(QString(), QString(),
                    dbus_server_name_, "actaulText",
                    this,
                    SLOT(setDataToTextEdit(long long, const QString&, const QString&)));
}


bool DbusServer::registrateNewServer()
{
    return registrate_new_server;
}


void DbusServer::uptedeTextInTextEdit()
{
    //  Отправляем сигнал для передачи нам актуального текста

    QDBusMessage msg = QDBusMessage::createSignal("/", dbus_server_name_, "getActualText");

    long long pid = (long long) QThread::currentThreadId();

    msg << pid;
    QDBusConnection::sessionBus().send(msg);
}


void DbusServer::connectCalculateCount()
{
    QDBusConnection session = QDBusConnection::sessionBus();

    session.connect(QString(), QString(),
                    dbus_server_name_, "textEditStart",
                    this, SLOT(teStart()));

    session.connect(QString(), QString(),
                    dbus_server_name_, "textEditStop",
                    this, SLOT(teStop()));
}


void DbusServer::focusWindowChanged(QWindow *focusWindow)
{
    if(focusWindow && !have_actual_text)
        uptedeTextInTextEdit();
}


void DbusServer::setDataToTextEdit(long long pid_adresat, const QString &file_name, const QString &html)
{
    long long curr_pid = (long long) QThread::currentThreadId();

    if(pid_adresat != curr_pid && pid_adresat != 0 && !registrate_new_server)
        return;

    te_->setDBusData(file_name, html);
    have_actual_text = true;
}


void DbusServer::checkActualText(long long pid)
{
    if(! have_actual_text || (long long) QThread::currentThreadId() == pid)
        return;

    //  Отправка актуального текста на запросивший его pid
    QDBusMessage msg = QDBusMessage::createSignal("/", dbus_server_name_, "actaulText");

    msg << pid << te_->fileName() << te_->html();

    QDBusConnection::sessionBus().send(msg);
    have_actual_text = false;
}

void DbusServer::lastWindowClosed()
{
    //  Если это дополнительный экземпляр в сессии
    if( ! registrate_new_server)
    {
        checkActualText(0);
        QDBusMessage msg = QDBusMessage::createSignal("/", dbus_server_name_, "textEditStop");
        QDBusConnection::sessionBus().send(msg);
        return;
    }

    //  Если основной
    te_count--;

    if( te_count == 0)
        exit(1);
}

void DbusServer::teStart()
{
    ++te_count;
}

void DbusServer::teStop()
{
    --te_count;

    if( te_count == 0)
        exit(1);
}
