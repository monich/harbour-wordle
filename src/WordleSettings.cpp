/*
 * Copyright (C) 2022-2023 Slava Monich <slava@monich.com>
 * Copyright (C) 2022 Jolla Ltd.
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

#include "WordleSettings.h"

#include "WordleDefs.h"
#include "WordleLanguage.h"
#include "WordleSearchEngine.h"

#include "HarbourDebug.h"

#include <QtCore/QLocale>
#include <QtCore/QUrl>

#include <MGConfItem>

#if QT_VERSION >= 0x040800
#define LOCALE_NAME (QLocale().bcp47Name()) // e.g. "en-GB"
#define LOCALE_SEPARATOR '-'
#else
#define LOCALE_NAME (QLocale().name()) // e.g. "en_GB"
#define LOCALE_SEPARATOR '_'
#endif

#define DCONF_KEY(x)                "/apps/" APP_NAME "/" x
#define KEY_LANGUAGE                DCONF_KEY("language")
#define KEY_SEARCH_ENGINE           DCONF_KEY("searchEngine")
#define KEY_WHATS_THIS              DCONF_KEY("whatsThis")
#define KEY_KEEP_DISPLAY_ON         DCONF_KEY("keepDisplayOn")
#define KEY_SHOW_PLAY_TIME          DCONF_KEY("showPlayTime")
#define KEY_ORIENTATION             DCONF_KEY("orientation")

#define DEFAULT_SEARCH_ENGINE       QString()
#define DEFAULT_WHATS_THIS          false
#define DEFAULT_KEEP_DISPLAY_ON     false
#define DEFAULT_SHOW_PLAY_TIME      true
#define DEFAULT_ORIENTATION         OrientationAny

// ==========================================================================
// WordleSettings::Private
// ==========================================================================

class WordleSettings::Private
{
public:
    static const QString DEFAULT_LANGUAGE_CODE;

    Private(WordleSettings*);

    static Orientation toOrientation(int);
    WordleLanguage validateLanguage(const QString);
    WordleLanguage language();
    void setLanguage(const QString);

public:
    const QList<WordleLanguage> iLanguages;
    WordleLanguage iDefaultLanguage;
    MGConfItem* iLanguage;
    MGConfItem* iSearchEngine;
    MGConfItem* iWhatsThis;
    MGConfItem* iKeepDisplayOn;
    MGConfItem* iShowPlayTime;
    MGConfItem* iOrientation;
};

const QString WordleSettings::Private::DEFAULT_LANGUAGE_CODE("en");

WordleSettings::Private::Private(
    WordleSettings* aParent) :
    iLanguages(WordleLanguage::availableLanguages()),
    iDefaultLanguage(DEFAULT_LANGUAGE_CODE),
    iLanguage(new MGConfItem(KEY_LANGUAGE, aParent)),
    iSearchEngine(new MGConfItem(KEY_SEARCH_ENGINE, aParent)),
    iWhatsThis(new MGConfItem(KEY_WHATS_THIS, aParent)),
    iKeepDisplayOn(new MGConfItem(KEY_KEEP_DISPLAY_ON, aParent)),
    iShowPlayTime(new MGConfItem(KEY_SHOW_PLAY_TIME, aParent)),
    iOrientation(new MGConfItem(KEY_ORIENTATION, aParent))
{
    QObject::connect(iLanguage, SIGNAL(valueChanged()),
        aParent, SIGNAL(languageChanged()));
    QObject::connect(iSearchEngine, SIGNAL(valueChanged()),
        aParent, SIGNAL(searchEngineChanged()));
    QObject::connect(iWhatsThis, SIGNAL(valueChanged()),
        aParent, SIGNAL(whatsThisChanged()));
    QObject::connect(iKeepDisplayOn, SIGNAL(valueChanged()),
        aParent, SIGNAL(keepDisplayOnChanged()));
    QObject::connect(iShowPlayTime, SIGNAL(valueChanged()),
        aParent, SIGNAL(showPlayTimeChanged()));
    QObject::connect(iOrientation, SIGNAL(valueChanged()),
        aParent, SIGNAL(orientationChanged()));

    const QString language(LOCALE_NAME);
    HDEBUG("System language" << language);
    const int sep = language.indexOf(LOCALE_SEPARATOR);
    const QString name((sep > 0) ? language.left(sep) : language);
    const int n = iLanguages.count();
    for (int i = 0; i < n; i++) {
        if (!iLanguages.at(i).getCode().compare(name, Qt::CaseInsensitive)) {
            HDEBUG("Using" << name << "as the default language");
            iDefaultLanguage = iLanguages.at(i);
            break;
        }
    }
}

WordleSettings::Orientation
WordleSettings::Private::toOrientation(
    int aValue)
{
    switch (aValue) {
    case OrientationAny:
    case OrientationPortrait:
    case OrientationLandscape:
        return (Orientation) aValue;
    }
    return DEFAULT_ORIENTATION;
}

WordleLanguage
WordleSettings::Private::validateLanguage(
    const QString aLanguage)
{
    const QString languageCode(aLanguage.toLower());
    const int n = iLanguages.count();
    for (int i = 0; i < n; i++) {
        if (iLanguages.at(i).getCode() == languageCode) {
            return iLanguages.at(i);
        }
    }
    HDEBUG("Falling back to" << iDefaultLanguage.getName());
    return iDefaultLanguage;
}

WordleLanguage
WordleSettings::Private::language()
{
    return validateLanguage(iLanguage->value(iDefaultLanguage.getCode()).toString());
}

void
WordleSettings::Private::setLanguage(
    const QString aValue)
{
    iLanguage->set(validateLanguage(aValue).getCode());
}

// ==========================================================================
// WordleSettings
// ==========================================================================

WordleSettings::WordleSettings(
    QObject* aParent) :
    QObject(aParent),
    iPrivate(new Private(this))
{
}

WordleSettings::~WordleSettings()
{
    delete iPrivate;
}

// Callback for qmlRegisterSingletonType<WordleSettings>
QObject*
WordleSettings::createSingleton(
    QQmlEngine*,
    QJSEngine*)
{
    return new WordleSettings;
}

QString
WordleSettings::searchUrl(
    QString aString)
{
    return WordleSearchEngine::getEngine(searchEngine()).queryUrl().
        arg(QString::fromUtf8(QUrl::toPercentEncoding(aString)));
}

QString
WordleSettings::language() const
{
    return iPrivate->language().getCode();
}

QString
WordleSettings::languageName() const
{
    return iPrivate->language().getName();
}

void
WordleSettings::setLanguage(
    QString aValue)
{
    HDEBUG(aValue);
    iPrivate->setLanguage(aValue);
}

QString
WordleSettings::searchEngine() const
{
    return iPrivate->iSearchEngine->value(DEFAULT_SEARCH_ENGINE).toString();
}

void
WordleSettings::setSearchEngine(
    QString aValue)
{
    HDEBUG(aValue);
    iPrivate->iSearchEngine->set(aValue);
}

bool
WordleSettings::whatsThis() const
{
    return iPrivate->iWhatsThis->value(DEFAULT_WHATS_THIS).toBool();
}

void
WordleSettings::setWhatsThis(
    bool aValue)
{
    HDEBUG(aValue);
    iPrivate->iWhatsThis->set(aValue);
}

bool
WordleSettings::keepDisplayOn() const
{
    return iPrivate->iKeepDisplayOn->value(DEFAULT_KEEP_DISPLAY_ON).toBool();
}

void
WordleSettings::setKeepDisplayOn(
    bool aValue)
{
    HDEBUG(aValue);
    iPrivate->iKeepDisplayOn->set(aValue);
}

bool
WordleSettings::showPlayTime() const
{
    return iPrivate->iShowPlayTime->value(DEFAULT_SHOW_PLAY_TIME).toBool();
}

void
WordleSettings::setShowPlayTime(
    bool aValue)
{
    HDEBUG(aValue);
    iPrivate->iShowPlayTime->set(aValue);
}

WordleSettings::Orientation
WordleSettings::orientation() const
{
    bool ok;
    const int value = iPrivate->iOrientation->value(DEFAULT_ORIENTATION).toInt(&ok);

    return ok ? Private::toOrientation(value) : DEFAULT_ORIENTATION;
}

void
WordleSettings::setOrientation(
    int aValue)
{
    HDEBUG(aValue);
    iPrivate->iOrientation->set(Private::toOrientation(aValue));
}
