/*
 * Copyright (C) 2022 Jolla Ltd.
 * Copyright (C) 2022 Slava Monich <slava@monich.com>
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
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *   3. Neither the names of the copyright holders nor the names of its
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
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

class WordleGame : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString language READ getLanguage WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(QString answer READ getAnswer NOTIFY answerChanged)
    Q_PROPERTY(QStringList keypad READ getKeypad NOTIFY keypadChanged)
    Q_PROPERTY(bool loading READ isLoading NOTIFY loadingChanged)
    Q_PROPERTY(int gameState READ getGameState NOTIFY gameStateChanged)
    Q_PROPERTY(int fullRows READ getFullRows NOTIFY fullRowsChanged)
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

    QString getLanguage() const;
    void setLanguage(QString aLanguageCode);

    bool isLoading() const;
    QString getAnswer() const;
    QStringList getKeypad() const;
    GameState getGameState() const;
    int getFullRows() const;
    bool canInputLetter() const;
    bool canDeleteLastLetter() const;
    bool canSubmitInput() const;

    Q_INVOKABLE int knownLetterState(QString aLetter);
    Q_INVOKABLE bool inputLetter(QString aLetter);
    Q_INVOKABLE void deleteLastLetter();
    Q_INVOKABLE bool submitInput();
    Q_INVOKABLE void newGame();

    // QAbstractItemModel
    QHash<int,QByteArray> roleNames() const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex& aParent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex& aIndex, int aRole) const Q_DECL_OVERRIDE;

Q_SIGNALS:
    void inputSubmitted(QString word);
    void languageChanged();
    void answerChanged();
    void keypadChanged();
    void loadingChanged();
    void gameStateChanged();
    void fullRowsChanged();
    void canInputLetterChanged();
    void canDeleteLastLetterChanged();
    void canSubmitInputChanged();

private:
    class Private;
    Private* iPrivate;
};

#endif // WORDLE_GAME_H
