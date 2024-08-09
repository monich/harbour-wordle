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

#ifndef WORDLE_H
#define WORDLE_H

#include "WordleTypes.h"
#include "WordleDefs.h"

#include <QColor>
#include <QDir>
#include <QObject>

class QQmlEngine;
class QJSEngine;

class Wordle :
    public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool darkOnLight READ getDarkOnLight WRITE setDarkOnLight NOTIFY darkOnLightChanged)
    Q_PROPERTY(QColor textColor READ getTextColor NOTIFY textColorChanged)
    Q_PROPERTY(QColor keyTextColor READ getKeyTextColor NOTIFY keyTextColorChanged)
    Q_PROPERTY(QColor emptySlotBorderColor READ getEmptySlotBorderColor NOTIFY emptySlotBorderColorChanged)
    Q_PROPERTY(QColor emptySlotBackgroundColor READ getEmptySlotBackgroundColor NOTIFY emptySlotBackgroundColorChanged)
    Q_PROPERTY(QColor defaultKeyBackgroundColor READ getDefaultKeyBackgroundColor NOTIFY defaultKeyBackgroundColorChanged)
    Q_PROPERTY(QColor notPresentBackgroundColor READ getNotPresentBackgroundColor NOTIFY notPresentBackgroundColorChanged)
    Q_PROPERTY(QColor presentBackgroundColor READ getPresentBackgroundColor NOTIFY presentBackgroundColorChanged)
    Q_PROPERTY(QColor presentHereBackgroundColor READ getPresentHereBackgroundColor NOTIFY presentHereBackgroundColorChanged)
    Q_ENUMS(LetterState)
    Q_ENUMS(Constants)

public:
    enum Constants {
        WordLength = WORDLE_WORD_LENGTH,
        MaxAttempts = WORDLE_MAX_ATTEMPTS
    };

    enum LetterState {
        LetterStateUnknown,
        LetterStateNotPresent,
        LetterStatePresent,
        LetterStatePresentHere
    };

    explicit Wordle(QObject* aParent = Q_NULLPTR);
    ~Wordle();

    // Callback for qmlRegisterSingletonType<Wordle>
    static QObject* createSingleton(QQmlEngine*, QJSEngine*);

    bool getDarkOnLight() const;
    void setDarkOnLight(bool aDarkOnLight);

    QColor getTextColor() const;
    QColor getKeyTextColor() const;
    QColor getEmptySlotBorderColor() const;
    QColor getEmptySlotBackgroundColor() const;
    QColor getDefaultKeyBackgroundColor() const;
    QColor getNotPresentBackgroundColor() const;
    QColor getPresentBackgroundColor() const;
    QColor getPresentHereBackgroundColor() const;

    Q_INVOKABLE QColor letterBackgroundColor(LetterState);
    Q_INVOKABLE static int functionalKeyCount(QString);
    Q_INVOKABLE static bool isFunctionalKey(QString);

    // Static utilities
    static QDir dataDir();
    static LetterState letterState(const QString, const QString, int);

Q_SIGNALS:
    void darkOnLightChanged();
    void textColorChanged();
    void keyTextColorChanged();
    void emptySlotBorderColorChanged();
    void emptySlotBackgroundColorChanged();
    void defaultKeyBackgroundColorChanged();
    void notPresentBackgroundColorChanged();
    void presentBackgroundColorChanged();
    void presentHereBackgroundColorChanged();

private:
    class Private;
    Private* iPrivate;
};

#endif // WORDLE_H
