/*
 * Copyright (C) 2022-2024 Slava Monich <slava@monich.com>
 * Copyright (C) 2022 Jolla Ltd.
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

#ifndef WORDLE_SETTINGS_H
#define WORDLE_SETTINGS_H

#include <QObject>

class QQmlEngine;
class QJSEngine;

class WordleSettings :
    public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(bool keepDisplayOn READ keepDisplayOn WRITE setKeepDisplayOn NOTIFY keepDisplayOnChanged)
    Q_PROPERTY(bool showPlayTime READ showPlayTime WRITE setShowPlayTime NOTIFY showPlayTimeChanged)
    Q_PROPERTY(int orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)
    Q_ENUMS(Orientation)

public:
    enum Orientation {
        OrientationAny,
        OrientationPortrait,
        OrientationLandscape
    };

    explicit WordleSettings(QObject* aParent = Q_NULLPTR);
    ~WordleSettings();

    // Callback for qmlRegisterSingletonType<WordleSettings>
    static QObject* createSingleton(QQmlEngine*, QJSEngine*);

    QString language() const;
    void setLanguage(const QString);

    bool keepDisplayOn() const;
    void setKeepDisplayOn(bool);

    bool showPlayTime() const;
    void setShowPlayTime(bool);

    Orientation orientation() const;
    void setOrientation(int);

Q_SIGNALS:
    void languageChanged();
    void keepDisplayOnChanged();
    void showPlayTimeChanged();
    void orientationChanged();

private:
    class Private;
    Private* iPrivate;
};

#endif // WORDLE_SETTINGS_H
