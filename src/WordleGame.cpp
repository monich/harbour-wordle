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
    first(Letter,letter) \
    last(State,state)

#define MODEL_ROLES(role) \
    MODEL_ROLES_(role,role,role)

#define MODEL_ROLE(X) X##Role

// s(SignalName,signalName)
#define MODEL_SIGNALS(s) \
    s(Language,language) \
    s(Playing,playing) \
    s(SecondsPlayed,secondsPlayed) \
    s(StartTime,startTime) \
    s(FinishTime,finishTime) \
    s(Answer,answer) \
    s(GameState,gameState) \
    s(FullRows,fullRows) \
    s(CanInputLetter,canInputLetter) \
    s(CanDeleteLastLetter,canDeleteLastLetter) \
    s(CanSubmitInput,canSubmitInput)
    // languageChanged is a special case (it's not queued)

#if HARBOUR_DEBUG
QDebug
operator<<(
    QDebug aDebug,
    WordleGame::GameState aState)
{
    switch (aState) {
    case WordleGame::GameInProgress: return (aDebug << "GameInProgress");
    case WordleGame::GameLost: return (aDebug << "GameLost");
    case WordleGame::GameWon: return (aDebug << "GameWon");
    }
    return (aDebug << (int)aState);
}
#endif // HARBOUR_DEBUG

// ==========================================================================
// WordleGame::Private
// ==========================================================================

class WordleGame::Private :
    public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Private)

public:
    typedef void (WordleGame::*SignalEmitter)();
    typedef uint SignalMask;
    enum Signal {
#define SIGNAL_ENUM_(Name,name) Signal##Name##Changed,
        MODEL_SIGNALS(SIGNAL_ENUM_)
#undef  SIGNAL_ENUM_
        SignalCount
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

    class State {
    public:
        State(Private* aPrivate);
        void queueSignals(Private* aPrivate) const;
    public:
        const GameState iGameState;
        const QDateTime iStartTime;
        const QDateTime iFinishTime;
        const QString iAnswer;
        const int iSecondsPlayed;
        const int iFullRows;
        const bool iCanSubmitInput;
        const bool iCanInputLetter;
        const bool iCanDeleteLastLetter;
    };

    enum {
        NumSlots = Wordle::WordLength * Wordle::MaxAttempts
    };

    static const QString DATE_TIME_FORMAT;
    static const QString STATE_FILE;
    static const QString STATE_KEY_ANSWER;
    static const QString STATE_KEY_INPUT;
    static const QString STATE_KEY_ATTEMPTS;
    static const QString STATE_KEY_SECS_PLAYED;
    static const QString STATE_KEY_TIME_START;  // UTC
    static const QString STATE_KEY_TIME_FINISH; // UTC

    Private(WordleGame* aParent);
    ~Private();

    WordleGame* parentGame();
    void queueSignal(Signal);
    bool signalQueued(Signal);
    void emitQueuedSignals();
    void setLanguage(const QString);
    void setPlaying(bool);
    void updatePlayTime();
    GameState gameState();
    bool canInputLetter();
    bool canDeleteLastLetter();
    bool canSubmitInput();
    void newGame();
    void saveState();
    void readState();
    void writeState();

    Wordle::LetterState letterState(const QString, int) const;
    void updateStateMap(const QString);
    int letterCount() const;

    static QString toString(const QDateTime);
    static QDateTime dateTimeValue(const QVariantMap, const QString);

private Q_SLOTS:
    void flushChanges();
    void onPlayTimerExpired();
    void onSaveTimerExpired();

public:
    SignalMask iQueuedSignals;
    Signal iFirstQueuedSignal;
    WordleLanguage iLanguage;
    QDateTime iStartTime;
    QDateTime iFinishTime;
    QString iAnswer;
    QString iInput;
    QStringList iAttempts;
    int iLoading;
    bool iPlaying;
    int iSecondsPlayed;
    int iSecondsPlayedThisTime;
    qint64 iPlayingStarted; // msec since epoch
    QMap<QChar,Wordle::LetterState> iStateMap;
    QTimer* iPlayTimer;
    QTimer* iSaveTimer;
    QTimer* iHoldoffTimer;
    QString iStateFile;
    QDir iDataDir;
};

const QString WordleGame::Private::DATE_TIME_FORMAT("yyyy-MM-dd hh:mm:ss");
const QString WordleGame::Private::STATE_FILE("state");
const QString WordleGame::Private::STATE_KEY_ANSWER("answer");
const QString WordleGame::Private::STATE_KEY_INPUT("input");
const QString WordleGame::Private::STATE_KEY_ATTEMPTS("attempts");
const QString WordleGame::Private::STATE_KEY_SECS_PLAYED("t");
const QString WordleGame::Private::STATE_KEY_TIME_START("t1");
const QString WordleGame::Private::STATE_KEY_TIME_FINISH("t2");

WordleGame::Private::State::State(Private* aPrivate) :
    iGameState(aPrivate->gameState()),
    iStartTime(aPrivate->iStartTime),
    iFinishTime(aPrivate->iFinishTime),
    iAnswer(aPrivate->iAnswer),
    iSecondsPlayed(aPrivate->iSecondsPlayed + aPrivate->iSecondsPlayedThisTime),
    iFullRows(aPrivate->iAttempts.count()),
    iCanSubmitInput(aPrivate->canSubmitInput()),
    iCanInputLetter(aPrivate->canInputLetter()),
    iCanDeleteLastLetter(aPrivate->canDeleteLastLetter())
{
}

void
WordleGame::Private::State::queueSignals(
    Private* aPrivate) const
{
    if (!aPrivate->signalQueued(SignalGameStateChanged) &&
        iGameState != aPrivate->gameState()) {
        HDEBUG("Game state" << iGameState << "=>" << aPrivate->gameState());
        aPrivate->queueSignal(SignalGameStateChanged);
    }
    if (!aPrivate->signalQueued(SignalStartTimeChanged) &&
        iStartTime != aPrivate->iStartTime) {
        aPrivate->queueSignal(SignalStartTimeChanged);
    }
    if (!aPrivate->signalQueued(SignalFinishTimeChanged) &&
        iFinishTime != aPrivate->iFinishTime) {
        aPrivate->queueSignal(SignalFinishTimeChanged);
    }
    if (iSecondsPlayed != (aPrivate->iSecondsPlayed + aPrivate->iSecondsPlayedThisTime)) {
        aPrivate->queueSignal(SignalSecondsPlayedChanged);
    }
    if (iFullRows != aPrivate->iAttempts.count()) {
        aPrivate->queueSignal(SignalFullRowsChanged);
    }
    if (iCanSubmitInput != aPrivate->canSubmitInput()) {
        aPrivate->queueSignal(SignalCanSubmitInputChanged);
    }
    if (iCanInputLetter != aPrivate->canInputLetter()) {
        aPrivate->queueSignal(SignalCanInputLetterChanged);
    }
    if (iCanDeleteLastLetter != aPrivate->canDeleteLastLetter()) {
        aPrivate->queueSignal(SignalCanDeleteLastLetterChanged);
    }
}

WordleGame::Private::Private(
    WordleGame* aParent) :
    QObject(aParent),
    iQueuedSignals(0),
    iFirstQueuedSignal(SignalCount),
    iLanguage(""), // Invalid language
    iLoading(0),
    iPlaying(false),
    iSecondsPlayed(0),
    iSecondsPlayedThisTime(0),
    iPlayingStarted(0),
    iPlayTimer(new QTimer(this)),
    iSaveTimer(new QTimer(this)),
    iHoldoffTimer(new QTimer(this)),
    iDataDir(QStandardPaths::writableLocation
        (QStandardPaths::GenericDataLocation) +
            QLatin1String("/" APP_NAME "/"))
{
    iPlayTimer->setSingleShot(true);
    connect(iPlayTimer, SIGNAL(timeout()), SLOT(onPlayTimerExpired()));
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
    bool dirty = false;

    updatePlayTime();
    if (iPlayTimer->isActive()) {
        iPlayTimer->stop();
        dirty = true;
    }
    if (iSaveTimer->isActive()) {
        iSaveTimer->stop();
        dirty = true;
    }
    if (dirty) {
        writeState();
    }
}

inline
WordleGame*
WordleGame::Private::parentGame()
{
    return qobject_cast<WordleGame*>(parent());
}

void
WordleGame::Private::queueSignal(
    Signal aSignal)
{
    if (aSignal >= 0 && aSignal < SignalCount) {
        const SignalMask signalBit = (SignalMask(1) << aSignal);

        if (iQueuedSignals) {
            iQueuedSignals |= signalBit;
            if (iFirstQueuedSignal > aSignal) {
                iFirstQueuedSignal = aSignal;
            }
        } else {
            iQueuedSignals = signalBit;
            iFirstQueuedSignal = aSignal;
        }
    }
}

bool
WordleGame::Private::signalQueued(
    Signal aSignal)
{
    return (iQueuedSignals & (SignalMask(1) << aSignal)) != 0;
}

void
WordleGame::Private::emitQueuedSignals()
{
    static const SignalEmitter emitSignal [] = {
#define SIGNAL_EMITTER_(Name,name) &WordleGame::name##Changed,
        MODEL_SIGNALS(SIGNAL_EMITTER_)
#undef  SIGNAL_EMITTER_
    };
    Q_STATIC_ASSERT(sizeof(emitSignal)/sizeof(emitSignal[0]) == SignalCount);
    if (iQueuedSignals) {
        WordleGame* model = parentGame();
        // Reset first queued signal before emitting the signals.
        // Signal handlers may emit more signals.
        uint i = iFirstQueuedSignal;
        iFirstQueuedSignal = SignalCount;
        for (; i < SignalCount && iQueuedSignals; i++) {
            const SignalMask signalBit = (SignalMask(1) << i);
            if (iQueuedSignals & signalBit) {
                iQueuedSignals &= ~signalBit;
                Q_EMIT (model->*(emitSignal[i]))();
            }
        }
    }
}

inline
QString
WordleGame::Private::toString(
    const QDateTime aTime)
{
    return aTime.toUTC().toString(DATE_TIME_FORMAT);
}

inline
QDateTime
WordleGame::Private::dateTimeValue(
    const QVariantMap aData,
    const QString aKey)
{
    QDateTime dt;
    const QString str(aData.value(aKey).toString());
    if (!str.isEmpty()) {
        dt = QDateTime::fromString(str, DATE_TIME_FORMAT);
        if (dt.isValid()) {
            // Time is stored in UTC
            dt.setTimeSpec(Qt::OffsetFromUTC);
            dt = dt.toTimeSpec(Qt::LocalTime);
        }
    }
    return dt;
}

void
WordleGame::Private::readState()
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
    const State prevState(this);
    if (HarbourJson::load(iStateFile, state)) {
        bool saveNow = false, ok;
        int i;

        iStateMap.clear();
        iAttempts.clear();
        iAnswer = state.value(STATE_KEY_ANSWER).toString().toLower();
        if (iAnswer.length() != Wordle::WordLength) {
            iAnswer = iLanguage.randomWord();
            saveNow = true;
        }
        iInput = state.value(STATE_KEY_INPUT).toString().toLower();
        if (iInput.length() > Wordle::WordLength) {
            iInput.resize(Wordle::WordLength);
            saveNow = true;
        }
        const QStringList attempts(state.value(STATE_KEY_ATTEMPTS).toStringList());
        const int n = attempts.count();
        for (i = 0; i < n; i++) {
            const QString word(attempts.at(i).toLower());
            if (word.length() == Wordle::WordLength) {
                updateStateMap(word);
                iAttempts.append(word);
            } else {
                saveNow = true;
            }
        }

        i = state.value(STATE_KEY_SECS_PLAYED).toInt(&ok);
        iSecondsPlayed = ok ? qMax(i, 0) : 0;
        iSecondsPlayedThisTime = 0;

        iStartTime = dateTimeValue(state, STATE_KEY_TIME_START);
        if (iStartTime.isValid()) {
            HDEBUG("game started" <<
                qPrintable(iStartTime.toString(DATE_TIME_FORMAT)));
        } else {
            iStartTime = QDateTime::currentDateTime();
            saveNow = true;
        }

        iFinishTime = dateTimeValue(state, STATE_KEY_TIME_FINISH);
        if (iFinishTime.isValid()) {
            if (gameState() == GameInProgress) {
                iFinishTime = QDateTime();
                saveNow = true;
            } else {
                HDEBUG("game finished" <<
                    qPrintable(iFinishTime.toString(DATE_TIME_FORMAT)));
            }
        } else if (gameState() != GameInProgress) {
            iFinishTime = QDateTime::currentDateTime();
            saveNow = true;
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
    prevState.queueSignals(this);
    game->endResetModel();

    if (!(--iLoading)) {
        Q_EMIT game->loadingChanged();
    }
}

void
WordleGame::Private::writeState()
{
    QVariantMap state;
    state.insert(STATE_KEY_ANSWER, iAnswer);
    state.insert(STATE_KEY_ATTEMPTS, iAttempts);
    state.insert(STATE_KEY_INPUT, iInput);
    state.insert(STATE_KEY_SECS_PLAYED, iSecondsPlayed + iSecondsPlayedThisTime);
    state.insert(STATE_KEY_TIME_START, toString(iStartTime));
    if (iFinishTime.isValid()) {
        state.insert(STATE_KEY_TIME_FINISH, toString(iFinishTime));
    }
    if (HarbourJson::save(iStateFile, state)) {
        HDEBUG(qPrintable(iStateFile));
    }
}

void
WordleGame::Private::saveState()
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

void
WordleGame::Private::flushChanges()
{
    if (iSaveTimer->isActive()) {
        iSaveTimer->stop();
        writeState();
    }
}

void
WordleGame::Private::onSaveTimerExpired()
{
    iHoldoffTimer->start();
    writeState();
}

void
WordleGame::Private::onPlayTimerExpired()
{
    updatePlayTime();
    emitQueuedSignals();
}

void
WordleGame::Private::updatePlayTime()
{
    if (iPlayingStarted) {
        const qint64 now = QDateTime::currentMSecsSinceEpoch();
        const qint64 secs = (now - iPlayingStarted)/1000;

        if (iSecondsPlayedThisTime < secs) {
            iSecondsPlayedThisTime = secs;
            if (!(iSecondsPlayedThisTime % 60)) {
                // We can sometimes miss it but it's fine
                saveState();
            }
            queueSignal(SignalSecondsPlayedChanged);
        }
        iPlayTimer->start(1000 - (now % 1000));
    } else {
        iPlayTimer->stop();
    }
}

void
WordleGame::Private::setLanguage(
    QString aLangCode)
{
    WordleLanguage language(aLangCode);
    if (language.isValid() && !iLanguage.equals(language)) {
        HDEBUG(aLangCode);
        queueSignal(SignalLanguageChanged);
        iLanguage = language;
        readState();
    }
}

void
WordleGame::Private::setPlaying(
    bool aPlaying)
{
    if (iPlaying != aPlaying) {
        iPlaying = aPlaying;
        HDEBUG(aPlaying);
        queueSignal(SignalPlayingChanged);
        if (iPlaying) {
            iSecondsPlayedThisTime = 0;
            iPlayingStarted = QDateTime::currentMSecsSinceEpoch();
            iPlayTimer->start(1000 - (iPlayingStarted % 1000));
        } else {
            iSecondsPlayed += iSecondsPlayedThisTime;
            iSecondsPlayedThisTime = 0;
            iPlayingStarted = 0;
            iPlayTimer->stop();
            saveState();
        }
    }
}

void
WordleGame::Private::newGame()
{
    const Private::State prevState(this);
    const int count = letterCount();

    if (count > 0) {
        WordleGame* model = parentGame();

        iAttempts.clear();
        iInput.resize(0);
        iStateMap.clear();
        // Change the state first
        const QVector<int> stateRole(1, Private::StateRole);
        const QVector<int> letterRole(1, Private::LetterRole);
        const QModelIndex topLeft(model->index(0));
        const QModelIndex bottomRight(model->index(count - 1));
        Q_EMIT model->dataChanged(topLeft, bottomRight, stateRole);
        Q_EMIT model->dataChanged(topLeft, bottomRight, letterRole);
    }
    iAnswer = iLanguage.randomWord();
    iStartTime = QDateTime::currentDateTime();
    iFinishTime = QDateTime();
    iSecondsPlayed = iSecondsPlayedThisTime = 0;
    if (iPlaying) {
        iPlayingStarted = QDateTime::currentMSecsSinceEpoch();
        iPlayTimer->start(1000 - (iPlayingStarted % 1000));
    } else {
        iPlayingStarted = 0;
        iPlayTimer->stop();
    }
    prevState.queueSignals(this);
    updatePlayTime();
}

WordleGame::GameState
WordleGame::Private::gameState()
{
    return iAttempts.contains(iAnswer) ? GameWon :
        (iAttempts.count() == Wordle::MaxAttempts) ? GameLost :
        GameInProgress;
}

bool
WordleGame::Private::canDeleteLastLetter()
{
    return iInput.length() > 0;
}

bool
WordleGame::Private::canInputLetter()
{
    return iAttempts.count() < Wordle::MaxAttempts &&
        iInput.length() < Wordle::WordLength &&
        !iAttempts.contains(iAnswer);
}

bool
WordleGame::Private::canSubmitInput()
{
    return iAttempts.count() < Wordle::MaxAttempts &&
        iInput.length() == Wordle::WordLength;
}

Wordle::LetterState
WordleGame::Private::letterState(
    const QString aWord,
    int aPos) const
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

int
WordleGame::Private::letterCount() const
{
    return iAttempts.count() * Wordle::WordLength + iInput.length();
}

void
WordleGame::Private::updateStateMap(
    const QString aWord)
{
    const int n = aWord.length();

    HASSERT(n == Wordle::WordLength);
    for (int i = 0; i < n; i++) {
        const QChar letter(aWord.at(i));
        const Wordle::LetterState oldState = iStateMap.value(letter);

        if (oldState < Wordle::LetterStatePresentHere) {
            const Wordle::LetterState newState = letterState(aWord, i);

            if (newState > oldState) {
                iStateMap.insert(letter, newState);
            }
        }
    }
}

// ==========================================================================
// WordleGame
// ==========================================================================

WordleGame::WordleGame(
    QObject* aParent) :
    QAbstractListModel(aParent),
    iPrivate(new Private(this))
{
}

WordleGame::~WordleGame()
{
    delete iPrivate;
}

QString
WordleGame::language() const
{
    return iPrivate->iLanguage.getCode();
}

QStringList
WordleGame::keypad() const
{
    return iPrivate->iLanguage.getKeypad();
}

void
WordleGame::setLanguage(
    const QString aLanguageCode)
{
    iPrivate->setLanguage(aLanguageCode);
    iPrivate->emitQueuedSignals();
}

bool
WordleGame::playing() const
{
    return iPrivate->iPlaying;
}

void
WordleGame::setPlaying(
    bool aPlaying)
{
    iPrivate->setPlaying(aPlaying);
    iPrivate->emitQueuedSignals();
}

bool
WordleGame::loading() const
{
    return iPrivate->iLoading > 0;
}

QDateTime
WordleGame::startTime() const
{
    return iPrivate->iStartTime;
}

QDateTime
WordleGame::finishTime() const
{
    return iPrivate->iFinishTime;
}

QString
WordleGame::answer() const
{
    return iPrivate->iAnswer;
}

QHash<int,QByteArray>
WordleGame::roleNames() const
{
    QHash<int,QByteArray> roles;
#define ROLE(X,x) roles.insert(Private::MODEL_ROLE(X), #x);
MODEL_ROLES(ROLE)
#undef ROLE
    return roles;
}

int
WordleGame::rowCount(
    const QModelIndex& aParent) const
{
    return Private::NumSlots;
}

QVariant
WordleGame::data(
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
            case Private::LetterRole: return word.mid(pos, 1);
            case Private::StateRole: return iPrivate->letterState(word, pos);
            }
        } else if ((i - attempted) < iPrivate->iInput.length()) {
            switch (role) {
            case Private::LetterRole: return QString(iPrivate->iInput.at(i - attempted));
            case Private::StateRole: return Wordle::LetterStateUnknown;
            }
        } else {
            switch (role) {
            case Private::LetterRole: return QString();
            case Private::StateRole: return Wordle::LetterStateUnknown;
            }
        }
    }
    return QVariant();
}

WordleGame::GameState
WordleGame::gameState() const
{
    return iPrivate->gameState();
}

int
WordleGame::secondsPlayed() const
{
    return iPrivate->iSecondsPlayed + iPrivate->iSecondsPlayedThisTime;
}

int
WordleGame::fullRows() const
{
    return iPrivate->iAttempts.count();
}

bool
WordleGame::canDeleteLastLetter() const
{
    return iPrivate->canDeleteLastLetter();
}

bool
WordleGame::canInputLetter() const
{
    return iPrivate->canInputLetter();
}

bool
WordleGame::canSubmitInput() const
{
    return iPrivate->canSubmitInput();
}

int
WordleGame::knownLetterState(
    const QString aLetter)
{
    return (aLetter.length() == 1) ?
        iPrivate->iStateMap.value(aLetter.at(0)) :
        Wordle::LetterStateUnknown;
}

bool
WordleGame::inputLetter(
    const QString aLetter)
{
    if (aLetter.length() == 1 && canInputLetter()) {
        const QChar letter(aLetter.at(0).toLower());
        const Private::State prevState(iPrivate);
        const QModelIndex modelIndex(index(iPrivate->letterCount()));

        HDEBUG(QString(letter));
        iPrivate->iInput.append(letter);
        Q_EMIT dataChanged(modelIndex, modelIndex);
        prevState.queueSignals(iPrivate);
        iPrivate->emitQueuedSignals();
        iPrivate->saveState();
        return true;
    }
    return false;
}

void
WordleGame::deleteLastLetter()
{
    const int len = iPrivate->iInput.length();
    if (len > 0) {
        const Private::State prevState(iPrivate);

        iPrivate->iInput.resize(len - 1);
        const QModelIndex idx(index(iPrivate->letterCount()));
        const QVector<int> role(1, Private::LetterRole);
        Q_EMIT dataChanged(idx, idx, role);

        prevState.queueSignals(iPrivate);
        iPrivate->emitQueuedSignals();
        iPrivate->saveState();
    }
}

bool
WordleGame::submitInput()
{
    if (iPrivate->canSubmitInput() &&
        iPrivate->iLanguage.isAllowed(iPrivate->iInput)) {
        const int count = iPrivate->letterCount();
        const QString word(iPrivate->iInput);
        const Private::State prevState(iPrivate);

        iPrivate->iAttempts.append(word);
        iPrivate->iInput.resize(0);
        iPrivate->updateStateMap(word);

        const QVector<int> role(1, Private::StateRole);
        Q_EMIT dataChanged(index(count - Wordle::WordLength), index(count - 1), role);
        Q_EMIT inputSubmitted(word);
        if (iPrivate->gameState() != GameInProgress) {
            HDEBUG("Game over");
            if (!iPrivate->iFinishTime.isValid()) {
                iPrivate->iFinishTime = QDateTime::currentDateTime();
            }
        }
        prevState.queueSignals(iPrivate);
        iPrivate->emitQueuedSignals();
        iPrivate->saveState();
        return true;
    }
    return false;
}

void
WordleGame::newGame()
{
    iPrivate->newGame();
    iPrivate->emitQueuedSignals();
    iPrivate->saveState();
}

#include "WordleGame.moc"
