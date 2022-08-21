/*
 * Copyright (C) 2020-2022 Jolla Ltd.
 * Copyright (C) 2020-2022 Slava Monich <slava@monich.com>
 *
 * You may use this file under the terms of the BSD license as follows:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer
 *      in the documentation and/or other materials provided with the
 *      distribution.
 *   3. Neither the names of the copyright holders nor the names of its
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation
 * are those of the authors and should not be interpreted as representing
 * any official policies, either expressed or implied.
 */

#include "Wordle.h"
#include "WordleDefs.h"
#include "WordleGame.h"
#include "WordleLanguageModel.h"
#include "WordleSettings.h"

#include "HarbourDebug.h"
#include "HarbourDisplayBlanking.h"
#include "HarbourSystemState.h"

#include <sailfishapp.h>

#include <QGuiApplication>
#include <QtQuick>

#define APP_QML_IMPORT  "harbour.wordle"
#define APP_QML_IMPORT_V1 1
#define APP_QML_IMPORT_V2 0

#define REGISTER_SINGLETON(class,uri,v1,v2) \
    qmlRegisterSingletonType<class>(uri, v1, v2, #class, class::createSingleton)
#define REGISTER_TYPE(class,uri,v1,v2) \
    qmlRegisterType<class>(uri, v1, v2, #class)

static void register_types(const char* uri, int v1, int v2)
{
    REGISTER_SINGLETON(Wordle, uri, v1, v2);
    REGISTER_SINGLETON(WordleSettings, uri, v1, v2);
    REGISTER_SINGLETON(HarbourSystemState, uri, v1, v2);
    REGISTER_TYPE(WordleGame, uri, v1, v2);
    REGISTER_TYPE(WordleLanguageModel, uri, v1, v2);
    REGISTER_TYPE(HarbourDisplayBlanking, uri, v1, v2);
}

int main(int argc, char *argv[])
{
    QGuiApplication* app = SailfishApp::application(argc, argv);

    app->setApplicationName(APP_NAME);
    register_types(APP_QML_IMPORT, APP_QML_IMPORT_V1, APP_QML_IMPORT_V2);

    // Load translations
    QLocale locale;
    QTranslator* tr = new QTranslator(app);
#ifdef OPENREPOS
    // OpenRepos build has settings applet
    const QString transDir("/usr/share/translations");
#else
    const QString transDir = SailfishApp::pathTo("translations").toLocalFile();
#endif
    const QString transFile(APP_NAME);
    if (tr->load(locale, transFile, "-", transDir) ||
        tr->load(transFile, transDir)) {
        app->installTranslator(tr);
    } else {
        HDEBUG("Failed to load translator for" << locale);
        delete tr;
    }

    // Initialize RNG
    qsrand((uint)QTime::currentTime().msec());

    // Create the view
    QQuickView* view = SailfishApp::createView();

    // Initialize the view and show it
    view->setTitle("Wordle");
    view->setSource(SailfishApp::pathTo("qml/main.qml"));
    view->showFullScreen();

    int ret = app->exec();

    delete view;
    delete app;
    return ret;
}
