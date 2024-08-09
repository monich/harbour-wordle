/*
 * Copyright (C) 2024 Slava Monich <slava@monich.com>
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

#ifndef MEEGO_WORDLE_H
#define MEEGO_WORDLE_H

#include <QApplication>
#include <QColor>
#include <QDeclarativeView>
#include <QLocale>
#include <QTranslator>
#include <QUrl>

class MeegoView :
    public QDeclarativeView
{
    Q_OBJECT

public:
    explicit MeegoView(QWidget* aParent = NULL);
    void show();
};

class MeegoApp
{
public:
    static QApplication* application(int&, char**);
    static QUrl pathTo(const QString&);
    static MeegoView* createView();
};

bool loadTranslations(QTranslator*, QLocale&,
    QString, QString, QString, QString aSuffix = QString());

class MeegoTheme :
    public QObject
{
    Q_OBJECT
    Q_PROPERTY(int paddingSmall READ paddingSmall CONSTANT)
    Q_PROPERTY(int paddingMedium READ paddingMedium CONSTANT)
    Q_PROPERTY(int paddingLarge READ paddingLarge CONSTANT)
    Q_PROPERTY(int horizontalPageMargin READ paddingLarge CONSTANT)
    Q_PROPERTY(int iconSizeSmall READ iconSizeSmall CONSTANT)
    Q_PROPERTY(int iconSizeMedium READ iconSizeMedium CONSTANT)
    Q_PROPERTY(int itemSizeExtraSmall READ itemSizeExtraSmall CONSTANT)
    Q_PROPERTY(int itemSizeSmall READ itemSizeSmall CONSTANT)
    Q_PROPERTY(int itemSizeMedium READ itemSizeMedium CONSTANT)
    Q_PROPERTY(int itemSizeHuge READ itemSizeHuge CONSTANT)
    Q_PROPERTY(int fontSizeExtraSmall READ fontSizeExtraSmall CONSTANT)
    Q_PROPERTY(int fontSizeSmall READ fontSizeSmall CONSTANT)
    Q_PROPERTY(int fontSizeMedium READ fontSizeMedium CONSTANT)
    Q_PROPERTY(int fontSizeLarge READ fontSizeLarge CONSTANT)
    Q_PROPERTY(int fontSizeExtraLarge READ fontSizeExtraLarge CONSTANT)
    Q_PROPERTY(int minimumPressHighlightTime READ minimumPressHighlightTime CONSTANT)
    Q_PROPERTY(QColor primaryColor READ primaryColor CONSTANT)
    Q_PROPERTY(QColor secondaryColor READ secondaryColor CONSTANT)
    Q_PROPERTY(QColor highlightColor READ secondaryColor CONSTANT)
    Q_PROPERTY(QColor secondaryHighlightColor READ secondaryColor CONSTANT)

public:
    explicit MeegoTheme(QObject* aParent) :
        QObject (aParent) {}

    static int paddingSmall() { return 6; }
    static int paddingMedium() { return 12; }
    static int paddingLarge() { return 24; }
    static int iconSizeSmall() { return 32; }
    static int iconSizeMedium() { return 64; }
    static int itemSizeExtraSmall() { return 32; }
    static int itemSizeSmall() { return 48; }
    static int itemSizeMedium() { return 64; }
    static int itemSizeHuge() { return 80; }
    static int fontSizeExtraSmall() { return 18; }
    static int fontSizeSmall() { return 20; }
    static int fontSizeMedium() { return 24; }
    static int fontSizeLarge() { return 28; }
    static int fontSizeExtraLarge() { return 32; }
    static int minimumPressHighlightTime() { return 64; }
    static QColor primaryColor() { return QColor(Qt::white); }
    static QColor secondaryColor() { return QColor("#e0e1e2"); }
    Q_INVOKABLE static QColor rgba(QColor color, qreal opacity)
        { return QColor(color.red(), color.green(), color.blue(), 255*opacity); }
};

#endif // MEEGO_WORDLE_H
