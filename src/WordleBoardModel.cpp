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

#include "Wordle.h"
#include "WordleBoardModel.h"

#define MODEL_ROLES_(first,role,last) \
    first(Letter,letter) \
    last(State,state)

#define MODEL_ROLES(role) \
    MODEL_ROLES_(role,role,role)

#define MODEL_ROLE(X) X##Role

// ==========================================================================
// WordleBoardModel::Private
// ==========================================================================

class WordleBoardModel::Private
{
public:
    enum {
        NumSlots = Wordle::WordLength * Wordle::MaxAttempts
    };

    enum Role {
        #define FIRST(X,x) FirstRole = Qt::UserRole, MODEL_ROLE(X) = FirstRole,
        #define ROLE(X,x) MODEL_ROLE(X),
        #define LAST(X,x) MODEL_ROLE(X), LastRole = MODEL_ROLE(X)
        MODEL_ROLES_(FIRST,ROLE,LAST)
        #undef FIRST
        #undef ROLE
        #undef LAST
    };

    QString iAnswer;
    QString iAllAttempts;
    QStringList iAttempts;
};

// ==========================================================================
// WordleBoardModel
// ==========================================================================

WordleBoardModel::WordleBoardModel(
    QObject* aParent) :
    QAbstractListModel(aParent),
    iPrivate(new Private)
{
}

WordleBoardModel::~WordleBoardModel()
{
    delete iPrivate;
}

QString
WordleBoardModel::answer() const
{
    return iPrivate->iAnswer;
}

void
WordleBoardModel::setAnswer(
    QString aAnwser)
{
    if (iPrivate->iAnswer != aAnwser) {
        iPrivate->iAnswer = aAnwser;
        Q_EMIT dataChanged(index(0), index(WORDLE_MAX_ATTEMPTS-1));
        Q_EMIT answerChanged();
    }
}

QString
WordleBoardModel::attempts() const
{
    return iPrivate->iAllAttempts;
}

void
WordleBoardModel::setAttempts(
    QString aAttempts)
{
    if (iPrivate->iAllAttempts != aAttempts) {
        iPrivate->iAllAttempts = aAttempts;
        iPrivate->iAttempts.clear();
        // Split the string
        const int n = aAttempts.length() / Wordle::WordLength;
        for (int i = 0; i < n; i++) {
            iPrivate->iAttempts.append(aAttempts.mid(i * Wordle::WordLength,
                Wordle::WordLength));
        }
        Q_EMIT dataChanged(index(0), index(WORDLE_MAX_ATTEMPTS-1));
        Q_EMIT attemptsChanged();
    }
}

QHash<int,QByteArray>
WordleBoardModel::roleNames() const
{
    QHash<int,QByteArray> roles;
    #define ROLE(X,x) roles.insert(Private::MODEL_ROLE(X), #x);
    MODEL_ROLES(ROLE)
    #undef ROLE
    return roles;
}

int
WordleBoardModel::rowCount(
    const QModelIndex&) const
{
    return Private::NumSlots;
}

QVariant
WordleBoardModel::data(
    const QModelIndex& aIndex,
    int aRole) const
{
    const int i = aIndex.row();
    if (i >= 0 && i < Private::NumSlots) {
        Private::Role role = (Private::Role)aRole;
        const int n = iPrivate->iAttempts.count();
        const int attempted = n * Wordle::WordLength;
        if (i < attempted) {
            const int pos = i % Wordle::WordLength;
            const QString word(iPrivate->iAttempts.at(i/Wordle::WordLength));
            switch (role) {
            case Private::LetterRole:
                return word.mid(pos, 1);
            case Private::StateRole:
                return Wordle::letterState(iPrivate->iAnswer, word, pos);
            }
        } else {
            switch (role) {
            case Private::LetterRole:
                return QString();
            case Private::StateRole:
                return Wordle::LetterStateUnknown;
            }
        }
    }
    return QVariant();
}

