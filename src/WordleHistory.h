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

#ifndef WORDLE_HISTORY_H
#define WORDLE_HISTORY_H

#include "WordleTypes.h"

#include <QAbstractListModel>
#include <QDateTime>
#include <QStringList>

class WordleHistory :
    public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(int totalCount READ totalCount NOTIFY totalCountChanged)
    Q_PROPERTY(int winCount READ winCount NOTIFY winCountChanged)
    Q_PROPERTY(int lastAttempts READ lastAttempts NOTIFY lastAttemptsChanged)
    Q_PROPERTY(int totalSec READ totalSec NOTIFY totalSecChanged)
    Q_PROPERTY(int minGameSec READ minGameSec NOTIFY minGameSecChanged)
    Q_PROPERTY(int currentStreak READ currentStreak NOTIFY currentStreakChanged)
    Q_PROPERTY(int maxStreak READ maxStreak NOTIFY maxStreakChanged)
    Q_PROPERTY(QList<int> guessDistribution READ guessDistribution NOTIFY guessDistributionChanged)

public:
    WordleHistory(QObject* aParent = Q_NULLPTR);
    ~WordleHistory();

    QString language() const;
    void setLanguage(const QString);

    int totalCount() const;
    int winCount() const;
    int lastAttempts() const;
    int totalSec() const;
    int minGameSec() const;
    int currentStreak() const;
    int maxStreak() const;
    QList<int> guessDistribution() const;

    // QAbstractItemModel
    QHash<int,QByteArray> roleNames() const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex&) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex&, int) const Q_DECL_OVERRIDE;

    Q_INVOKABLE void add(const QString, const QStringList, QDateTime, QDateTime, int);
    Q_INVOKABLE void clear();

Q_SIGNALS:
    void languageChanged();
    void totalCountChanged();
    void winCountChanged();
    void lastAttemptsChanged();
    void totalSecChanged();
    void minGameSecChanged();
    void currentStreakChanged();
    void maxStreakChanged();
    void guessDistributionChanged();

private:
    class Private;
    Private* iPrivate;
};

#endif // WORDLE_HISTORY_H
