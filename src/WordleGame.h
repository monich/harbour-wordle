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

#ifndef WORDLE_GAME_H
#define WORDLE_GAME_H

#include <QAbstractListModel>
#include <QDateTime>
#include <QString>
#include <QStringList>

class WordleGame :
    public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(QStringList keypad READ keypad NOTIFY languageChanged)
    Q_PROPERTY(QStringList keypad1 READ keypad1 NOTIFY languageChanged)
    Q_PROPERTY(QStringList keypad2 READ keypad2 NOTIFY languageChanged)
    Q_PROPERTY(QDateTime startTime READ startTime NOTIFY finishTimeChanged)
    Q_PROPERTY(QDateTime finishTime READ finishTime NOTIFY finishTimeChanged)
    Q_PROPERTY(QString answer READ answer NOTIFY answerChanged)
    Q_PROPERTY(QStringList attempts READ attempts NOTIFY attemptsChanged)
    Q_PROPERTY(bool playing READ playing WRITE setPlaying NOTIFY playingChanged)
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
    Q_PROPERTY(int secondsPlayed READ secondsPlayed NOTIFY secondsPlayedChanged)
    Q_PROPERTY(int gameState READ gameState NOTIFY gameStateChanged)
    Q_PROPERTY(int fullRows READ fullRows NOTIFY fullRowsChanged)
    Q_PROPERTY(bool canInputLetter READ canInputLetter NOTIFY canInputLetterChanged)
    Q_PROPERTY(bool canDeleteLastLetter READ canDeleteLastLetter NOTIFY canDeleteLastLetterChanged)
    Q_PROPERTY(bool canSubmitInput READ canSubmitInput NOTIFY canSubmitInputChanged)
    Q_ENUMS(GameState)

public:
    enum GameState {
        GameInProgress,
        GameLost,
        GameWon
    };

    explicit WordleGame(QObject* aParent = Q_NULLPTR);
    ~WordleGame();

    QString language() const;
    void setLanguage(const QString);

    bool playing() const;
    void setPlaying(bool);

    bool loading() const;
    QDateTime startTime() const;
    QDateTime finishTime() const;
    QString answer() const;
    QStringList attempts() const;
    QStringList keypad() const;
    QStringList keypad1() const;
    QStringList keypad2() const;
    GameState gameState() const;
    int secondsPlayed() const;
    int fullRows() const;
    bool canInputLetter() const;
    bool canDeleteLastLetter() const;
    bool canSubmitInput() const;

    Q_INVOKABLE int knownLetterState(const QString);
    Q_INVOKABLE bool inputLetter(const QString);
    Q_INVOKABLE void deleteLastLetter();
    Q_INVOKABLE bool submitInput();
    Q_INVOKABLE void newGame();

    // QAbstractItemModel
    QHash<int,QByteArray> roleNames() const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex&) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex&, int) const Q_DECL_OVERRIDE;

Q_SIGNALS:
    void startTimeChanged();
    void finishTimeChanged();
    void languageChanged();
    void answerChanged();
    void attemptsChanged();
    void playingChanged();
    void loadingChanged();
    void secondsPlayedChanged();
    void gameStateChanged();
    void fullRowsChanged();
    void canInputLetterChanged();
    void canDeleteLastLetterChanged();
    void canSubmitInputChanged();
    void inputSubmitted(QString word);
    void gameOver();

private:
    class Private;
    Private* iPrivate;
};

#endif // WORDLE_GAME_H
