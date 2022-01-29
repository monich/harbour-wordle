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

#include "Wordle.h"
#include "WordleGame.h"
#include "WordleLanguage.h"

#include "HarbourJson.h"
#include "HarbourDebug.h"

#include <QStringList>
#include <QTimer>
#include <QDir>
#include <QStandardPaths>

// Strings and letters are lower case

#define MODEL_ROLES_(first,role,last) \
    role(Letter,letter) \
    last(State,state)

#define MODEL_ROLES(role) \
    MODEL_ROLES_(role,role,role)

#define MODEL_ROLE(X) X##Role

// ==========================================================================
// WordleGame::Private
// ==========================================================================

class WordleGame::Private : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Private)

public:
    enum Role {
#define FIRST(X,x) FirstRole = Qt::UserRole, MODEL_ROLE(X) = FirstRole,
#define ROLE(X,x) MODEL_ROLE(X),
#define LAST(X,x) MODEL_ROLE(X), LastRole = MODEL_ROLE(X)
        MODEL_ROLES_(FIRST,ROLE,LAST)
#undef FIRST
#undef ROLE
#undef LAST
    };

    enum {
        NumSlots = Wordle::WordLength * Wordle::MaxAttempts
    };

    static const QString STATE_FILE;
    static const QString STATE_KEY_ANSWER;
    static const QString STATE_KEY_INPUT;
    static const QString STATE_KEY_ATTEMPTS;

    Private(WordleGame* aParent);
    ~Private();

    WordleGame* parentGame();
    QString getLanguage();
    bool setLanguage(QString aLangCode);
    QStringList getKeypad();
    void saveState();
    void readState();
    void writeState();

    Wordle::LetterState letterState(const QString aWord, int aPos) const;
    void updateStateMap(const QString aWord);
    int letterCount() const;

private Q_SLOTS:
    void flushChanges();
    void onSaveTimerExpired();

public:
    WordleLanguage iLanguage;
    QString iAnswer;
    QString iInput;
    QStringList iAttempts;
    int iLoading;
    QMap<QChar,Wordle::LetterState> iStateMap;
    QTimer* iSaveTimer;
    QTimer* iHoldoffTimer;
    QString iStateFile;
    QDir iDataDir;
};

const QString WordleGame::Private::STATE_FILE("state");
const QString WordleGame::Private::STATE_KEY_ANSWER("answer");
const QString WordleGame::Private::STATE_KEY_INPUT("input");
const QString WordleGame::Private::STATE_KEY_ATTEMPTS("attempts");

WordleGame::Private::Private(WordleGame* aParent) :
    QObject(aParent),
    iLanguage(""), // Invalid language
    iLoading(0),
    iSaveTimer(new QTimer(this)),
    iHoldoffTimer(new QTimer(this)),
    iDataDir(QStandardPaths::writableLocation
        (QStandardPaths::GenericDataLocation) +
            QLatin1String("/" APP_NAME "/"))
{
    // Current state is saved at least every 10 seconds
    iSaveTimer->setInterval(10000);
    iSaveTimer->setSingleShot(true);
    connect(iSaveTimer, SIGNAL(timeout()), SLOT(onSaveTimerExpired()));
    // And not more often than every second
    iHoldoffTimer->setInterval(1000);
    iHoldoffTimer->setSingleShot(true);
    connect(iHoldoffTimer, SIGNAL(timeout()), SLOT(flushChanges()));
}

WordleGame::Private::~Private()
{
    flushChanges();
}

inline WordleGame* WordleGame::Private::parentGame()
{
    return qobject_cast<WordleGame*>(parent());
}

void WordleGame::Private::readState()
{
    WordleGame* game = parentGame();

    if (!(iLoading++)) {
        Q_EMIT game->loadingChanged();
    }
    QDir langDir(iDataDir.absoluteFilePath(iLanguage.getCode()));
    if (!langDir.mkpath(QStringLiteral("."))) {
        HWARN("Failed to create" << langDir.absolutePath() << "directory");
    }
    iStateFile = langDir.absoluteFilePath(STATE_FILE);

    game->beginResetModel();

    QVariantMap state;
    if (HarbourJson::load(iStateFile, state)) {
        bool saveNow = false;

        iStateMap.clear();
        iAttempts.clear();
        iAnswer = state.value(STATE_KEY_ANSWER).toString().toLower();
        if (iAnswer.length() != Wordle::WordLength) {
            iAnswer = iLanguage.randomWord();
            saveNow = true;
        }
        iInput = state.value(STATE_KEY_INPUT).toString().toLower();
        const QStringList attempts(state.value(STATE_KEY_ATTEMPTS).toStringList());
        const int n = attempts.count();
        for (int i = 0; i < n; i++) {
            const QString word(attempts.at(i).toLower());
            if (word.length() == Wordle::WordLength) {
                updateStateMap(word);
                iAttempts.append(word);
            }
        }
        if (saveNow) {
            saveState();
        }
    } else {
        iAnswer = iLanguage.randomWord();
        iAttempts.clear();
        iInput.clear();
        saveState();
    }

    game->endResetModel();

    if (!(--iLoading)) {
        Q_EMIT game->loadingChanged();
    }
}

void WordleGame::Private::writeState()
{
    QVariantMap state;
    state.insert(STATE_KEY_ANSWER, iAnswer);
    state.insert(STATE_KEY_ATTEMPTS, iAttempts);
    state.insert(STATE_KEY_INPUT, iInput);
    if (HarbourJson::save(iStateFile, state)) {
        HDEBUG(qPrintable(iStateFile));
    }
}

void WordleGame::Private::saveState()
{
    if (!iStateFile.isEmpty()) {
        if (!iHoldoffTimer->isActive()) {
            // Idle state, write the file right away
            iSaveTimer->stop();
            writeState();
        } else {
            // Make sure it eventually gets saved even if changes will
            // keep happening in quick succession
            if (!iSaveTimer->isActive()) {
                iSaveTimer->start();
            }
        }
        // Restart hold off timer
        iHoldoffTimer->start();
    }
}

void WordleGame::Private::flushChanges()
{
    if (iSaveTimer->isActive()) {
        iSaveTimer->stop();
        writeState();
    }
}

void WordleGame::Private::onSaveTimerExpired()
{
    iHoldoffTimer->start();
    writeState();
}

bool WordleGame::Private::setLanguage(QString aLangCode)
{
    WordleLanguage language(aLangCode);
    if (language.isValid() && !iLanguage.equals(language)) {
        iLanguage = language;
        HDEBUG(aLangCode);
        readState();
        return true;
    }
    return false;
}

QString WordleGame::Private::getLanguage()
{
    return iLanguage.getCode();
}

QStringList WordleGame::Private::getKeypad()
{
    return iLanguage.getKeypad();
}

Wordle::LetterState WordleGame::Private::letterState(const QString aWord, int aPos) const
{
    if (aPos >= 0 && aPos < Wordle::WordLength) {
        const QChar* answer = iAnswer.constData();
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
            return Wordle::LetterStatePresentHere;
        } else if (!iAnswer.contains(letter)) {
            return Wordle::LetterStateNotPresent;
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
                return Wordle::LetterStatePresent;
            } else {
                // Skip aPos, it's already taken care of
                for (i++; i < Wordle::WordLength; i++) {
                    if (word[i] != letter && answer[i] == letter) {
                        misPlaceCount++;
                        if (misPlaceCount > misPlaceIndex) {
                            return Wordle::LetterStatePresent;
                        }
                    }
                }
                return Wordle::LetterStateNotPresent;
            }
        }
    }
    return Wordle::LetterStateUnknown;
}

int WordleGame::Private::letterCount() const
{
    return iAttempts.count() * Wordle::WordLength + iInput.length();
}

void WordleGame::Private::updateStateMap(const QString aWord)
{
    const int n = aWord.length();
    HASSERT(n == Wordle::WordLength);
    for (int i = 0; i < n; i++) {
        const QChar letter(aWord.at(i));
        Wordle::LetterState oldState = iStateMap.value(letter);
        Wordle::LetterState newState = letterState(aWord, i);
        if (newState > oldState) {
            iStateMap.insert(letter, newState);
        }
    }
}

// ==========================================================================
// WordleGame
// ==========================================================================

WordleGame::WordleGame(QObject* aParent) :
    QAbstractListModel(aParent),
    iPrivate(new Private(this))
{
}

WordleGame::~WordleGame()
{
    delete iPrivate;
}

QString WordleGame::getLanguage() const
{
    return iPrivate->getLanguage();
}

void WordleGame::setLanguage(QString aLanguageCode)
{
    const GameState prevState = getGameState();
    const int prevFullRows = getFullRows();
    const bool couldSubmitInput = canSubmitInput();
    const bool couldInputLetter = canInputLetter();
    const bool couldDeleteLastLetter = canDeleteLastLetter();

    if (iPrivate->setLanguage(aLanguageCode)) {
        Q_EMIT languageChanged();
        Q_EMIT keypadChanged();
        if (prevFullRows != getFullRows()) {
            Q_EMIT fullRowsChanged();
        }
        if (canSubmitInput() != couldSubmitInput) {
            Q_EMIT canSubmitInputChanged();
        }
        if (canInputLetter() != couldInputLetter) {
            Q_EMIT canInputLetterChanged();
        }
        if (canDeleteLastLetter() != couldDeleteLastLetter) {
            Q_EMIT canDeleteLastLetterChanged();
        }
        if (getGameState() != prevState) {
            Q_EMIT gameStateChanged();
        }
    }
}

bool WordleGame::isLoading() const
{
    return iPrivate->iLoading > 0;
}

QString WordleGame::getAnswer() const
{
    return iPrivate->iAnswer;
}

QStringList WordleGame::getKeypad() const
{
    return iPrivate->getKeypad();
}

QHash<int,QByteArray> WordleGame::roleNames() const
{
    QHash<int,QByteArray> roles;
#define ROLE(X,x) roles.insert(Private::MODEL_ROLE(X), #x);
MODEL_ROLES(ROLE)
#undef ROLE
    return roles;
}

int WordleGame::rowCount(const QModelIndex& aParent) const
{
    return Private::NumSlots;
}

QVariant WordleGame::data(const QModelIndex& aIndex, int aRole) const
{
    const int i = aIndex.row();
    if (i >= 0 && i < Private::NumSlots) {
        const int n = iPrivate->iAttempts.count();
        const int attempted = n * Wordle::WordLength;
        if (i < attempted) {
            const int pos = i % Wordle::WordLength;
            const QString word(iPrivate->iAttempts.at(i/Wordle::WordLength));
            switch ((Private::Role)aRole) {
            case Private::LetterRole: return word.mid(pos, 1);
            case Private::StateRole: return iPrivate->letterState(word, pos);
            }
        } else if ((i - attempted) < iPrivate->iInput.length()) {
            switch ((Private::Role)aRole) {
            case Private::LetterRole: return QString(iPrivate->iInput.at(i - attempted));
            case Private::StateRole: return Wordle::LetterStateUnknown;
            }
        } else {
            switch ((Private::Role)aRole) {
            case Private::LetterRole: return QString();
            case Private::StateRole: return Wordle::LetterStateUnknown;
            }
        }
    }
    return QVariant();
}

WordleGame::GameState WordleGame::getGameState() const
{
    return iPrivate->iAttempts.contains(iPrivate->iAnswer) ? GameWon :
        (iPrivate->iAttempts.count() == Wordle::MaxAttempts) ? GameLost :
        GameInProgress;
}

int WordleGame::getFullRows() const
{
    return iPrivate->iAttempts.count();
}

bool WordleGame::canDeleteLastLetter() const
{
    return iPrivate->iInput.length() > 0;
}

bool WordleGame::canInputLetter() const
{
    return iPrivate->iAttempts.count() < Wordle::MaxAttempts &&
        iPrivate->iInput.length() < Wordle::WordLength &&
        !iPrivate->iAttempts.contains(iPrivate->iAnswer);
}

bool WordleGame::canSubmitInput() const
{
    return iPrivate->iAttempts.count() < Wordle::MaxAttempts &&
        iPrivate->iInput.length() == Wordle::WordLength;
}

int WordleGame::knownLetterState(QString aLetter)
{
    return (aLetter.length() == 1) ?
        iPrivate->iStateMap.value(aLetter.at(0)) :
        Wordle::LetterStateUnknown;
}

bool WordleGame::inputLetter(QString aLetter)
{
    if (aLetter.length() == 1 && canInputLetter()) {
        const QChar letter(aLetter.at(0).toLower());
        HDEBUG(letter);
        const bool couldDeleteLastLetter = canDeleteLastLetter();
        const QModelIndex modelIndex(index(iPrivate->letterCount()));
        iPrivate->iInput.append(letter);
        Q_EMIT dataChanged(modelIndex, modelIndex);
        if (!canInputLetter()) {
            Q_EMIT canInputLetterChanged();
        }
        if (!couldDeleteLastLetter) {
            HASSERT(canDeleteLastLetter());
            Q_EMIT canDeleteLastLetterChanged();
        }
        if (canSubmitInput()) {
            Q_EMIT canSubmitInputChanged();
        }
        iPrivate->saveState();
        return true;
    }
    return false;
}

void WordleGame::deleteLastLetter()
{
    const int len = iPrivate->iInput.length();
    if (len > 0) {
        const bool couldSubmitInput = canSubmitInput();
        const bool couldInputLetter = canInputLetter();

        iPrivate->iInput.resize(len - 1);
        const QModelIndex idx(index(iPrivate->letterCount()));
        const QVector<int> role(1, Private::LetterRole);
        Q_EMIT dataChanged(idx, idx, role);

        if (couldSubmitInput) {
            HASSERT(!canSubmitInput());
            Q_EMIT canSubmitInputChanged();
        }
        if (canInputLetter() != couldInputLetter) {
            Q_EMIT canInputLetterChanged();
        }
        if (!canDeleteLastLetter()) {
            Q_EMIT canDeleteLastLetterChanged();
        }
        iPrivate->saveState();
    }
}

bool WordleGame::submitInput()
{
    if (canSubmitInput()) {
        if (iPrivate->iLanguage.isAllowed(iPrivate->iInput)) {
            const GameState prevState = getGameState();
            const int count = iPrivate->letterCount();
            const QString word(iPrivate->iInput);

            iPrivate->iAttempts.append(word);
            iPrivate->iInput.resize(0);
            iPrivate->updateStateMap(word);
            const QVector<int> role(1, Private::StateRole);
            Q_EMIT dataChanged(index(count - Wordle::WordLength), index(count - 1), role);
            Q_EMIT inputSubmitted(word);
            Q_EMIT fullRowsChanged();
            Q_EMIT canDeleteLastLetterChanged();
            if (!canSubmitInput()) {
                Q_EMIT canSubmitInputChanged();
            }
            if (canInputLetter()) {
                Q_EMIT canInputLetterChanged();
            }
            if (prevState != getGameState()) {
                HDEBUG("Game state" << prevState << "=>" << getGameState());
                Q_EMIT gameStateChanged();
            }
            iPrivate->saveState();
            return true;
        }
    }
    return false;
}

void WordleGame::newGame()
{
    const GameState prevState = getGameState();
    const int prevFullRows = getFullRows();
    const bool couldSubmitInput = canSubmitInput();
    const bool couldInputLetter = canInputLetter();
    const bool couldDeleteLastLetter = canDeleteLastLetter();

    beginResetModel();
    iPrivate->iAttempts.clear();
    iPrivate->iInput.resize(0);
    iPrivate->iStateMap.clear();
    endResetModel();

    if (prevFullRows) {
        HASSERT(!getFullRows());
        Q_EMIT fullRowsChanged();
    }
    if (couldSubmitInput) {
        HASSERT(!canSubmitInput());
        Q_EMIT canSubmitInputChanged();
    }
    if (!couldInputLetter) {
        HASSERT(canInputLetter());
        Q_EMIT canInputLetterChanged();
    }
    if (couldDeleteLastLetter) {
        HASSERT(!canDeleteLastLetter());
        Q_EMIT canDeleteLastLetterChanged();
    }
    if (getGameState() != prevState) {
        Q_EMIT gameStateChanged();
    }

    iPrivate->iAnswer = iPrivate->iLanguage.randomWord();
    Q_EMIT answerChanged();
    iPrivate->saveState();
}

#include "WordleGame.moc"
