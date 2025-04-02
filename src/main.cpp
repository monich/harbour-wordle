/*
 * Copyright (C) 2020-2025 Slava Monich <slava@monich.com>
 * Copyright (C) 2020-2022 Jolla Ltd.
 *
 * You may use this file under the terms of the BSD license as follows:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer
 *     in the documentation and/or other materials provided with the
 *     distribution.
 *
 *  3. Neither the names of the copyright holders nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
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
#include "WordleBoardModel.h"
#include "WordleDefs.h"
#include "WordleGame.h"
#include "WordleHistory.h"
#include "WordleLanguageModel.h"
#include "WordlePlatform.h"
#include "WordleSearchEngineModel.h"
#include "WordleSettings.h"

#include "HarbourDebug.h"
#include "HarbourDisplayBlanking.h"
#include "HarbourSystemState.h"

#if QT_VERSION >= 0x050000
#include <QtQuick>
#else
#include <QtDeclarative>
#endif

#ifdef HARMATTAN
#define PlatformView MeegoView
#else
#include "HarbourBattery.h"
#define loadTranslations(translator,locale,filename,prefix,directory) \
    ((translator)->load(locale,filename,prefix,directory))
#define REGISTER_SINGLETON(class,uri,v1,v2) \
    qmlRegisterSingletonType<class>(uri, v1, v2, #class, class::createSingleton)
#define PlatformView QQuickView
#endif

#define APP_QML_IMPORT  "harbour.wordle"
#define APP_QML_IMPORT_V1 1
#define APP_QML_IMPORT_V2 0

#define REGISTER_TYPE(class,uri,v1,v2) \
    qmlRegisterType<class>(uri, v1, v2, #class)

static void register_types(const char* uri, int v1, int v2)
{
    REGISTER_TYPE(WordleBoardModel, uri, v1, v2);
    REGISTER_TYPE(WordleGame, uri, v1, v2);
    REGISTER_TYPE(WordleHistory, uri, v1, v2);
    REGISTER_TYPE(WordleLanguageModel, uri, v1, v2);
    REGISTER_TYPE(WordleSearchEngineModel, uri, v1, v2);
    REGISTER_TYPE(HarbourDisplayBlanking, uri, v1, v2);
#ifdef HARMATTAN
    qmlRegisterType<Wordle>(uri, v1, v2, "Constants");
#else
    REGISTER_SINGLETON(HarbourBattery, uri, v1, v2);
    REGISTER_SINGLETON(HarbourSystemState, uri, v1, v2);
    REGISTER_SINGLETON(Wordle, uri, v1, v2);
    REGISTER_SINGLETON(WordleSettings, uri, v1, v2);
#endif
}

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QCoreApplication* app = PlatformApp::application(argc, argv);

    app->setApplicationName(APP_NAME);
    register_types(APP_QML_IMPORT, APP_QML_IMPORT_V1, APP_QML_IMPORT_V2);

    // Load translations
    QLocale locale;
    QTranslator* tr = new QTranslator(app);
    const QString transDir = PlatformApp::pathTo("translations").toLocalFile();
    const QString transFile(APP_NAME);
    if (loadTranslations(tr, locale, transFile, "-", transDir) ||
        tr->load(transFile, transDir)) {
        app->installTranslator(tr);
    } else {
        HDEBUG("Failed to load translator for" << locale);
        delete tr;
    }

    // Initialize RNG
    qsrand((uint)QTime::currentTime().msec());

    // Create the view
    PlatformView* view = PlatformApp::createView();

#ifdef HARMATTAN
    QDeclarativeContext* root = view->rootContext();
    QObject* theme = new MeegoTheme(root);
    root->setContextProperty("Wordle", new Wordle(root));
    root->setContextProperty("WordleSettings", new WordleSettings(root));
    root->setContextProperty("HarbourSystemState", new HarbourSystemState(root));
    // WordleTheme is used to avoid conflicts with Theme in com.nokia.meego
    root->setContextProperty("WordleTheme", theme);
    root->setContextProperty("Theme", theme);
#else
    view->setTitle("Wordle");
#endif

    view->setSource(PlatformApp::pathTo("qml/main.qml"));
    view->showFullScreen();

    int ret = app->exec();

    delete view;
    delete app;
    return ret;
}
