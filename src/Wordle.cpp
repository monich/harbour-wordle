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

#include "Wordle.h"

#include "HarbourDebug.h"

#include <QStandardPaths>

class Wordle::Private
{
public:
    class ColorScheme;

    Private();

    bool setDarkOnLight(bool);

public:
    bool iDarkOnLight;
    const ColorScheme* iColorScheme;
};

// ==========================================================================
// Wordle::Private::ColorScheme
// ==========================================================================

class Wordle::Private::ColorScheme
{
public:
    static const ColorScheme LIGHT_ON_DARK;
    static const ColorScheme DARK_ON_LIGHT;

    static const ColorScheme* colorScheme(bool);

public:
    const QColor iTextColor;
    const QColor iKeyTextColor;
    const QColor iEmptySlotBorderColor;
    const QColor iEmptySlotBackgroundColor;
    const QColor iDefaultKeyBackgroundColor;
    const QColor iNotPresentBackgroundColor;
    const QColor iPresentBackgroundColor;
    const QColor iPresentHereBackgroundColor;

public:
    ColorScheme(const QColor& aTextColor,
        const QColor& aKeyTextColor,
        const QColor& aEmptySlotBorderColor,
        const QColor& aEmptySlotBackgroundColor,
        const QColor& aDefaultKeyBackgroundColor,
        const QColor& aNotPresentBackgroundColor,
        const QColor& aPresentBackgroundColor,
        const QColor& aPresentHereBackgroundColor) :
        iTextColor(aTextColor),
        iKeyTextColor(aKeyTextColor),
        iEmptySlotBorderColor(aEmptySlotBorderColor),
        iEmptySlotBackgroundColor(aEmptySlotBackgroundColor),
        iDefaultKeyBackgroundColor(aDefaultKeyBackgroundColor),
        iNotPresentBackgroundColor(aNotPresentBackgroundColor),
        iPresentBackgroundColor(aPresentBackgroundColor),
        iPresentHereBackgroundColor(aPresentHereBackgroundColor)
        {}
};

const Wordle::Private::ColorScheme
Wordle::Private::ColorScheme::LIGHT_ON_DARK(
        QColor(Qt::white),          // textColor
        QColor(Qt::white),          // keyTextColor
        QColor(211, 214, 218, 112), // emptySlotBorderColor
        QColor(Qt::transparent),    // emptySlotBackgroundColor
        QColor("#787c7e"),          // defaultKeyBackgroundColor
        QColor("#3a3a3c"),          // notPresentBackgroundColor
        QColor("#b59f3b"),          // presentBackgroundColor
        QColor("#538d4e"));         // presentHereBackgroundColor

const Wordle::Private::ColorScheme
Wordle::Private::ColorScheme::DARK_ON_LIGHT(
        QColor(Qt::white),          // textColor
        QColor("#1a1a1b"),          // keyTextColor
        QColor(58, 58, 60, 112),    // emptySlotBorderColor
        QColor(Qt::transparent),    // emptySlotBackgroundColor
        QColor("#d3d6da"),          // defaultKeyBackgroundColor
        QColor("#787c7e"),          // notPresentBackgroundColor
        QColor("#c9b458"),          // presentBackgroundColor
        QColor("#6aaa64"));         // presentHereBackgroundColor

const Wordle::Private::ColorScheme*
Wordle::Private::ColorScheme::colorScheme(
    bool aDarkOnLight)
{
    return aDarkOnLight ? &ColorScheme::DARK_ON_LIGHT : &ColorScheme::LIGHT_ON_DARK;
}

// ==========================================================================
// Wordle::Private
// ==========================================================================

Wordle::Private::Private() :
    iDarkOnLight(false),
    iColorScheme(ColorScheme::colorScheme(iDarkOnLight))
{
}

bool
Wordle::Private::setDarkOnLight(
    bool aDarkOnLight)
{
    if (iDarkOnLight != aDarkOnLight) {
        iDarkOnLight = aDarkOnLight;
        iColorScheme = Private::ColorScheme::colorScheme(iDarkOnLight);
        return true;
    }
    return false;
}

// ==========================================================================
// Wordle
// ==========================================================================

Wordle::Wordle(QObject* aParent) :
    QObject(aParent),
    iPrivate(new Private)
{
}

Wordle::~Wordle()
{
    delete iPrivate;
}

// Callback for qmlRegisterSingletonType<Wordle>
QObject*
Wordle::createSingleton(
    QQmlEngine*,
    QJSEngine*)
{
    return new Wordle;
}

bool
Wordle::getDarkOnLight() const
{
    return iPrivate->iDarkOnLight;
}

void
Wordle::setDarkOnLight(
    bool aDarkOnLight)
{
    if (iPrivate->setDarkOnLight(aDarkOnLight)) {
        HDEBUG(aDarkOnLight);
        Q_EMIT darkOnLightChanged();
        Q_EMIT textColorChanged();
        Q_EMIT keyTextColorChanged();
        Q_EMIT emptySlotBorderColorChanged();
        Q_EMIT emptySlotBackgroundColorChanged();
        Q_EMIT defaultKeyBackgroundColorChanged();
        Q_EMIT notPresentBackgroundColorChanged();
        Q_EMIT presentBackgroundColorChanged();
        Q_EMIT presentHereBackgroundColorChanged();
    }
}

QColor
Wordle::getTextColor() const
{
    return iPrivate->iColorScheme->iTextColor;
}

QColor
Wordle::getKeyTextColor() const
{
    return iPrivate->iColorScheme->iKeyTextColor;
}

QColor
Wordle::getEmptySlotBorderColor() const
{
    return iPrivate->iColorScheme->iEmptySlotBorderColor;
}

QColor
Wordle::getEmptySlotBackgroundColor() const
{
    return iPrivate->iColorScheme->iEmptySlotBackgroundColor;
}

QColor
Wordle::getDefaultKeyBackgroundColor() const
{
    return iPrivate->iColorScheme->iDefaultKeyBackgroundColor;
}

QColor
Wordle::getNotPresentBackgroundColor() const
{
    return iPrivate->iColorScheme->iNotPresentBackgroundColor;
}

QColor
Wordle::getPresentBackgroundColor() const
{
    return iPrivate->iColorScheme->iPresentBackgroundColor;
}

QColor
Wordle::getPresentHereBackgroundColor() const
{
    return iPrivate->iColorScheme->iPresentHereBackgroundColor;
}

QColor
Wordle::letterBackgroundColor(
    LetterState aState)
{
    switch (aState) {
    case LetterStateNotPresent:
        return iPrivate->iColorScheme->iNotPresentBackgroundColor;
    case LetterStatePresent:
        return iPrivate->iColorScheme->iPresentBackgroundColor;
    case LetterStatePresentHere:
        return iPrivate->iColorScheme->iPresentHereBackgroundColor;
    case LetterStateUnknown:
        // default
        break;
    }
    return iPrivate->iColorScheme->iEmptySlotBackgroundColor;
}

int
Wordle::functionalKeyCount(
    QString aKeys)
{
    int count = 0;
    const int n = aKeys.length();
    for (int i = 0; i < n; i++) {
        if (isFunctionalKey(aKeys.at(i))) {
            count++;
        }
    }
    HDEBUG(aKeys << "=>" << count);
    return count;
}

bool
Wordle::isFunctionalKey(
    QString aKey)
{
    return !aKey.isEmpty() && (aKey.at(0).category() == QChar::Other_Control);
}

/* static  */
QDir
Wordle::dataDir()
{
    return QDir(QStandardPaths::writableLocation
        (QStandardPaths::GenericDataLocation) +
        QLatin1String("/" APP_NAME "/"));
}

/* static */
Wordle::LetterState
Wordle::letterState(
    const QString aAnswer,
    const QString aWord,
    int aPos)
{
    if (aPos >= 0 && aPos < WordLength) {
        const QChar* answer = aAnswer.constData();
        const QChar* word = aWord.constData();
        const QChar letter(word[aPos]);

        //
        // Number of letters colored as PresentHere and Present in
        // the guess word must not exceed the number of such letters
        // in the answer.
        //
        // Letters that are at the right positions are always marked
        // as such, and the misplaced ones are optionally marked from
        // left to right, until the total count is reached.
        //
        // For example, if the answer is WHOLE then only the first E
        // in WHEEL would be marked as present (but misplaced) and only
        // the second O in WOOLY would be marked as correct (sitting
        // at the right place), while the first O won't be marked as
        // present because that would exceed the total number of O's
        // in WHOLE.
        //
        if (answer[aPos] == letter) {
            return LetterStatePresentHere;
        } else if (!aAnswer.contains(letter)) {
            return LetterStateNotPresent;
        } else {
            int misPlaceIndex = 0; // This counts one at aPos
            int i, misPlaceCount = 0;

            for (i = 0; i < aPos; i++) {
                if (word[i] != answer[i]) {
                    if (word[i] == letter) {
                        misPlaceIndex++;
                    } else if (answer[i] == letter) {
                        misPlaceCount++;
                    }
                }
            }

            // At this point we have misPlaceIndex for the letter at aPos
            if (misPlaceCount > misPlaceIndex) {
                return LetterStatePresent;
            } else {
                // Skip aPos, it's already taken care of
                for (i++; i < WordLength; i++) {
                    if (word[i] != letter && answer[i] == letter) {
                        misPlaceCount++;
                        if (misPlaceCount > misPlaceIndex) {
                            return LetterStatePresent;
                        }
                    }
                }
                return LetterStateNotPresent;
            }
        }
    }
    return LetterStateUnknown;
}
