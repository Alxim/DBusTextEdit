/****************************************************************************
  **
  ** Copyright (C) 2016 The Qt Company Ltd.
  ** Contact: https://www.qt.io/licensing/
  **
  ** This file is part of the demonstration applications of the Qt Toolkit.
  **
  ** $QT_BEGIN_LICENSE:BSD$
  ** Commercial License Usage
  ** Licensees holding valid commercial Qt licenses may use this file in
  ** accordance with the commercial license agreement provided with the
  ** Software or, alternatively, in accordance with the terms contained in
  ** a written agreement between you and The Qt Company. For licensing terms
  ** and conditions see https://www.qt.io/terms-conditions. For further
  ** information use the contact form at https://www.qt.io/contact-us.
  **
  ** BSD License Usage
  ** Alternatively, you may use this file under the terms of the BSD license
  ** as follows:
  **
  ** "Redistribution and use in source and binary forms, with or without
  ** modification, are permitted provided that the following conditions are
  ** met:
  **   * Redistributions of source code must retain the above copyright
  **     notice, this list of conditions and the following disclaimer.
  **   * Redistributions in binary form must reproduce the above copyright
  **     notice, this list of conditions and the following disclaimer in
  **     the documentation and/or other materials provided with the
  **     distribution.
  **   * Neither the name of The Qt Company Ltd nor the names of its
  **     contributors may be used to endorse or promote products derived
  **     from this software without specific prior written permission.
  **
  **
  ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
  **
  ** $QT_END_LICENSE$
  **
  ****************************************************************************/

#include "textedit.h"
#include "dbusserver.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <iostream>
#include <QDebug>

QString parseArgument(const QStringList list, const QString& name);

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(textedit);

    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("Shchelikalin Maxim");
    QCoreApplication::setApplicationName(TARGET);
    QCoreApplication::setApplicationVersion(VERSION);

    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::applicationName());
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", QCoreApplication::translate("main", "Открыть файл. Например: file=:/example.html"));
    parser.addPositionalArgument("session", QCoreApplication::translate("main", "Задать имя для сессии синхронизации через D-Bus. Например session=abc"));


    QCommandLineOption detacheOption(QStringList() << "d" << "detache",
                  QCoreApplication::translate("main", "отключить синхронизацию через D-Bus"));
    parser.addOption(detacheOption);

    QCommandLineOption workOption(QStringList() << "w" << "workAsSystem",
                  QCoreApplication::translate("main", "сессия продолжится до перезагрузки"));
    parser.addOption(workOption);

    parser.process(a);

    TextEdit mw;

    const QRect availableGeometry = QApplication::desktop()->availableGeometry(&mw);
    mw.resize(availableGeometry.width() / 2, (availableGeometry.height() * 2) / 3);
    mw.move((availableGeometry.width() - mw.width()) / 2,
            (availableGeometry.height() - mw.height()) / 2);


    //  Выделение аругментов
    QStringList list = parser.positionalArguments();
    QString session_name = parseArgument(list, "session"),
            file_name  = parseArgument(list, "file");

    if(file_name.isEmpty())
        file_name = QLatin1String(":/example.html");

    if( ! session_name.isEmpty())
        QCoreApplication::setApplicationName(QString(TARGET) + " - session " + session_name);

    //  Проверка наличия опции отключения синхронизации
    if( parser.isSet(detacheOption) )
    {
        if (!mw.load(file_name))
           mw.fileNew();
        mw.show();
        return a.exec();
    }

    //  Начинает обрабатывать сценарий с сессиями
    DbusServer server(&mw, session_name);
    QObject::connect(&a, SIGNAL(focusWindowChanged(QWindow *)),
                     &server, SLOT(focusWindowChanged(QWindow*)));
    QObject::connect(&a, SIGNAL(lastWindowClosed()),
                     &server, SLOT(lastWindowClosed()));

    if(server.registrateNewServer())
    {
        if (!mw.load(file_name))
           mw.fileNew();

        a.setQuitOnLastWindowClosed(false);
        server.connectCalculateCount();

        if( parser.isSet(workOption) )
            server.teStart();
    }
    else
    {
        server.uptedeTextInTextEdit();
    }

    mw.show();
    return a.exec();
}


/*!
 * \brief parseArgument выделение аргументов
 * \param list перечень входящих аргументов
 * \param name имя искомого аргумента
 * \return значение аргумента, если агрумента нет, то вернётся пустая строка
 */

QString parseArgument(const QStringList list, const QString& name)
{
    QStringList list_filter = list.filter(name);
    QString result;

    if(list_filter.size() > 0)
    {
        result = list_filter.first();
        result.remove(name + "=");
    }
    return result;
}
