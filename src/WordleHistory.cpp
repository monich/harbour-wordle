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
#include "WordleHistory.h"
#include "WordleLanguage.h"

#include "HarbourDebug.h"

#include <QByteArray>
#include <QFileInfo>
#include <QFile>

#define MODEL_ROLES_(first,role,last) \
    first(Win,win) \
    role(Answer,answer) \
    role(Attempts,attempts) \
    role(StartTime,startTime) \
    role(EndTime,endTime) \
    last(SecondsPlayed,secondsPlayed)

#define MODEL_ROLE(X) X##Role

#define MODEL_ROLES(role) \
    MODEL_ROLES_(role,role,role)

// s(SignalName,signalName)
#define MODEL_SIGNALS(s) \
    s(Language,language) \
    s(TotalCount,totalCount) \
    s(WinCount,winCount) \
    s(LastAttempts,lastAttempts) \
    s(TotalSec,totalSec) \
    s(MinGameSec,minGameSec) \
    s(CurrentStreak,currentStreak) \
    s(MaxStreak,maxStreak) \
    s(GuessDistribution,guessDistribution)

// ==========================================================================
// WordleHistory::Private
// ==========================================================================

class WordleHistory::Private
{
    typedef quint64 Signature;
    static const QString HISTORY_FILE;
    static const Signature SIGNATURE;
    static const quint32 PADDING;

    struct HistoryEntry {
        // +========+======+======================+
        // | Offset | Size | Description          |
        // +========+======+======================+
        // | 0      | 5    | Answer               |
        // | 5      | 5    | Attempt 1            |
        // | 10     | 5    | Attempt 2 (or zeros) |
        // | 15     | 5    | Attempt 3 (or zeros) |
        // | 20     | 5    | Attempt 4 (or zeros) |
        // | 25     | 5    | Attempt 5 (or zeros) |
        // | 30     | 5    | Attempt 6 (or zeros) |
        // | 35     | 1    | Zero padding         |
        // +========+======+======================+
        ushort iState[36];      // See above ^
        quint64 iStartTime;     // Milliseconds since epoch
        quint64 iEndTime;       // Milliseconds since epoch
        quint32 iSecondsPlayed; // Total seconds actually played
        quint32 iPadding;       // Zero padding to 8-byte boundary
    };

    Q_STATIC_ASSERT(sizeof(HistoryEntry) == 96);

public:
    typedef void (WordleHistory::*SignalEmitter)();
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
        State(Private*);
        void queueSignals(Private*) const;

    public:
        int iGuessCounts[WORDLE_MAX_ATTEMPTS];
        const int iTotalCount;
        const int iWinCount;
        const int iLastAttempts;
        const int iTotalSec;
        const int iMinGameSec;
        const int iCurrentStreak;
        const int iMaxStreak;
    };

    Private(WordleHistory*);
    ~Private();

    void emitQueuedSignals();
    void setLanguage(const QString);
    void add(const QString, const QStringList, const QDateTime&, const QDateTime&, int);
    void clear();
    QVariant data(int, Role);

private:
    void queueSignal(Signal);
    void unmapHistoryFile();
    void closeHistoryFile();
    void readHistory();
    bool parseHistory();
    static bool isValid(const HistoryEntry*);
    static bool isWin(const HistoryEntry*);
    static int guessCount(const HistoryEntry*);
    static QString toString(const ushort*, int, int);

public:
    WordleLanguage iLanguage;
    const HistoryEntry* iHistoryEntries;
    int iGuessCounts[WORDLE_MAX_ATTEMPTS];
    int iTotalCount;
    int iWinCount;
    int iLastAttempts;
    int iTotalSec;
    int iMinGameSec;
    int iCurrentStreak;
    int iMaxStreak;

private:
    const QDir iDataDir;
    WordleHistory* iObject;
    SignalMask iQueuedSignals;
    Signal iFirstQueuedSignal;
    QFile iHistoryFile;
    uchar* iHistoryData;
};

const QString WordleHistory::Private::HISTORY_FILE("history");
const WordleHistory::Private::Signature // WRDLHST1 in little endian
WordleHistory::Private::SIGNATURE = 0x315453484c445257ull;
const quint32 WordleHistory::Private::PADDING = 0x3d3d3d3d; // '===='

WordleHistory::Private::State::State(Private* aPrivate) :
    iTotalCount(aPrivate->iTotalCount),
    iWinCount(aPrivate->iWinCount),
    iLastAttempts(aPrivate->iLastAttempts),
    iTotalSec(aPrivate->iTotalSec),
    iMinGameSec(aPrivate->iMinGameSec),
    iCurrentStreak(aPrivate->iCurrentStreak),
    iMaxStreak(aPrivate->iMaxStreak)
{
    memcpy(iGuessCounts, aPrivate->iGuessCounts, sizeof(iGuessCounts));
}

void
WordleHistory::Private::State::queueSignals(
    Private* aPrivate) const
{
    if (iTotalCount != aPrivate->iTotalCount) {
        aPrivate->queueSignal(SignalTotalCountChanged);
    }
    if (iWinCount != aPrivate->iWinCount) {
        aPrivate->queueSignal(SignalWinCountChanged);
    }
    if (iLastAttempts != aPrivate->iLastAttempts) {
        aPrivate->queueSignal(SignalLastAttemptsChanged);
    }
    if (iTotalSec != aPrivate->iTotalSec) {
        aPrivate->queueSignal(SignalTotalSecChanged);
    }
    if (iMinGameSec != aPrivate->iMinGameSec) {
        aPrivate->queueSignal(SignalMinGameSecChanged);
    }
    if (iCurrentStreak != aPrivate->iCurrentStreak) {
        aPrivate->queueSignal(SignalCurrentStreakChanged);
    }
    if (iMaxStreak != aPrivate->iMaxStreak) {
        aPrivate->queueSignal(SignalMaxStreakChanged);
    }
    if (memcmp(iGuessCounts, aPrivate->iGuessCounts, sizeof(iGuessCounts))) {
        aPrivate->queueSignal(SignalGuessDistributionChanged);
    }
}

WordleHistory::Private::Private(
    WordleHistory* aParent) :
    iHistoryEntries(Q_NULLPTR),
    iTotalCount(0),
    iWinCount(0),
    iLastAttempts(0),
    iTotalSec(0),
    iMinGameSec(0),
    iCurrentStreak(0),
    iMaxStreak(0),
    iDataDir(Wordle::dataDir()),
    iObject(aParent),
    iQueuedSignals(0),
    iFirstQueuedSignal(SignalCount),
    iHistoryData(Q_NULLPTR)
{
    memset(iGuessCounts, 0, sizeof(iGuessCounts));
}

WordleHistory::Private::~Private()
{
    closeHistoryFile();
}

void
WordleHistory::Private::queueSignal(
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

void
WordleHistory::Private::emitQueuedSignals()
{
    static const SignalEmitter emitSignal [] = {
        #define SIGNAL_EMITTER_(Name,name) &WordleHistory::name##Changed,
        MODEL_SIGNALS(SIGNAL_EMITTER_)
        #undef SIGNAL_EMITTER_
    };
    Q_STATIC_ASSERT(sizeof(emitSignal)/sizeof(emitSignal[0]) == SignalCount);
    if (iQueuedSignals) {
        // Reset first queued signal before emitting the signals.
        // Signal handlers may emit more signals.
        uint i = iFirstQueuedSignal;
        iFirstQueuedSignal = SignalCount;
        for (; i < SignalCount && iQueuedSignals; i++) {
            const SignalMask signalBit = (SignalMask(1) << i);
            if (iQueuedSignals & signalBit) {
                iQueuedSignals &= ~signalBit;
                Q_EMIT (iObject->*(emitSignal[i]))();
            }
        }
    }
}

void
WordleHistory::Private::setLanguage(
    QString aLangCode)
{
    WordleLanguage language(aLangCode);
    if (language.isValid()) {
        if (!iLanguage.equals(language)) {
            const State prevState(this);
            iObject->beginResetModel();
            HDEBUG(aLangCode);
            queueSignal(SignalLanguageChanged);
            iLanguage = language;

            // Update the file name (which depends on the language)
            const QDir dataDir(iDataDir.absoluteFilePath(language.getCode()));
            const QFileInfo historyFileInfo(dataDir, HISTORY_FILE);
            iHistoryFile.setFileName(historyFileInfo.absoluteFilePath());

            readHistory();
            prevState.queueSignals(this);
            iObject->endResetModel();
        }
    }
}

void
WordleHistory::Private::clear()
{
    const State prevState(this);
    iObject->beginResetModel();
    unmapHistoryFile();
    if (iHistoryFile.exists()) {
        // The file is closed before being removed
        HWARN("Removing" << qPrintable(iHistoryFile.fileName()));
        iHistoryFile.remove();
    }
    prevState.queueSignals(this);
    iObject->endResetModel();
}

void
WordleHistory::Private::add(
    const QString aAnswer,
    const QStringList aAttempts,
    const QDateTime& aStartTime,
    const QDateTime& aEndTime,
    int aSecondsPlayed)
{
    const State prevState(this);
    closeHistoryFile();

    if (!iHistoryFile.exists() || !iHistoryFile.size()) {
        QDir dataDir(iDataDir.absoluteFilePath(iLanguage.getCode()));
        if (!dataDir.mkpath(QStringLiteral("."))) {
            HWARN("Failed to create" << dataDir.absolutePath() << "directory");
        }
        if (iHistoryFile.open(QFile::ReadWrite)) {
            if (iHistoryFile.write((char*)&SIGNATURE, sizeof(SIGNATURE)) ==
                sizeof(SIGNATURE)) {
                HDEBUG("Created" << qPrintable(iHistoryFile.fileName()));
            } else {
                HDEBUG("Failed to initialize" <<
                    qPrintable(iHistoryFile.fileName()));
            }
        } else {
            HWARN("Failed to create" << qPrintable(iHistoryFile.fileName()));
        }
    } else if (!iHistoryFile.open(QFile::Append)) {
        HWARN("Failed to open" << qPrintable(iHistoryFile.fileName()) <<
            "error" << iHistoryFile.error());
    }

    iObject->beginResetModel();
    if (iHistoryFile.isOpen()) {
        // Set up the new entry
        HistoryEntry entry;
        memset(&entry, 0, sizeof(entry));
        // The answer and the attempts must be full words
        for (int i = 0; i < WORDLE_WORD_LENGTH; i++) {
            entry.iState[i] = aAnswer.at(i).unicode();
        }
        for (int k = 0; k < aAttempts.size(); k++) {
            const QString attempt(aAttempts.at(k));
            for (int i = 0; i < WORDLE_WORD_LENGTH; i++) {
                entry.iState[(k + 1) * WORDLE_WORD_LENGTH + i] =
                    attempt.at(i).unicode();
            }
        }
        entry.iStartTime = aStartTime.toMSecsSinceEpoch();
        entry.iEndTime = aEndTime.toMSecsSinceEpoch();
        entry.iSecondsPlayed = aSecondsPlayed;
        entry.iPadding = PADDING;

        HDEBUG("Writing" << qPrintable(iHistoryFile.fileName()));
        if (iHistoryFile.write((char*)&entry, sizeof(entry)) == sizeof(entry)) {
            iHistoryFile.close();
            readHistory();
        } else {
            HWARN("Failed to write" << qPrintable(iHistoryFile.fileName()) <<
                "error" << iHistoryFile.error());
        }
    }
    prevState.queueSignals(this);
    iObject->endResetModel();
}

void
WordleHistory::Private::unmapHistoryFile()
{
    if (iHistoryData) {
        iHistoryFile.unmap(iHistoryData);
        iHistoryData = Q_NULLPTR;
        iHistoryEntries = Q_NULLPTR;
        memset(iGuessCounts, 0, sizeof(iGuessCounts));
        iTotalCount = 0;
        iWinCount = 0;
        iLastAttempts = 0;
        iTotalSec = 0;
        iMinGameSec = 0;
        iCurrentStreak = 0;
        iMaxStreak = 0;
    }
}

void
WordleHistory::Private::closeHistoryFile()
{
    unmapHistoryFile();
    if (iHistoryFile.isOpen()) {
        iHistoryFile.close();
    }
}

void
WordleHistory::Private::readHistory()
{
    closeHistoryFile();

    const quint64 size = iHistoryFile.size();
    if (size >= sizeof(SIGNATURE)) {
        if (iHistoryFile.open(QFile::ReadOnly)) {
            iHistoryData = iHistoryFile.map(0, size);
            if (iHistoryData) {
                HDEBUG("Opened" << qPrintable(iHistoryFile.fileName()));
                if (*(Signature*)iHistoryData == SIGNATURE) {
                    if (parseHistory()) {
                        return;
                    }
                }
                HDEBUG("Deleting" << qPrintable(iHistoryFile.fileName()));
                unmapHistoryFile();
            } else {
                HWARN("Failed to map" << qPrintable(iHistoryFile.fileName()) <<
                    "error" << iHistoryFile.error());
            }
        }
        // The file is closed before being removed
        iHistoryFile.remove();
    } else {
        HDEBUG(qPrintable(iHistoryFile.fileName()) <<
            "doesn't exist (or is empty)");
    }
}

bool
WordleHistory::Private::parseHistory()
{
    // We already know that the file starts with SIGNATURE
    const quint64 size = iHistoryFile.size();
    if ((size - sizeof(SIGNATURE)) % sizeof(SIGNATURE)) {
        HDEBUG(qPrintable(iHistoryFile.fileName()) <<
            "has unexpected size" << size);
        return false;
    } else {
        int wins = 0, totalSec = 0, lastAttempts = 0, minGameSec = 0;
        int currentStreak = 0, maxStreak = 0;
        int guessCounts[WORDLE_MAX_ATTEMPTS];
        const int n = int(size / sizeof(HistoryEntry));
        const HistoryEntry* entries = (HistoryEntry*)(iHistoryData +
            sizeof(SIGNATURE));
        // Validate the entries and collect statistics
        memset(guessCounts, 0, sizeof(guessCounts));
        for (int i = 0; i < n; i++) {
            const HistoryEntry* entry = entries + i;
            if (isValid(entry)) {
                totalSec += entry->iSecondsPlayed;
                if (!minGameSec || (uint) minGameSec > entry->iSecondsPlayed) {
                    minGameSec = entry->iSecondsPlayed;
                }
                if (isWin(entry)) {
                    wins++;
                    currentStreak++;
                    if (maxStreak < currentStreak) {
                        maxStreak = currentStreak;
                    }
                    lastAttempts = guessCount(entry);
                    guessCounts[lastAttempts - 1]++;
                } else {
                    currentStreak = 0;
                    lastAttempts = 0;
                }
            } else {
                HDEBUG(qPrintable(iHistoryFile.fileName()) <<
                    "invalid entry #" << i);
                return false;
            }
        }
        HDEBUG(qPrintable(iHistoryFile.fileName()) << n << "entries");
        Q_STATIC_ASSERT(sizeof(guessCounts) == sizeof(iGuessCounts));
        memcpy(iGuessCounts, guessCounts, sizeof(iGuessCounts));
        iHistoryEntries = entries;
        iTotalCount = n;
        iWinCount = wins;
        iLastAttempts = lastAttempts;
        iTotalSec = totalSec;
        iMinGameSec = minGameSec;
        iCurrentStreak = currentStreak;
        iMaxStreak = maxStreak;
        return true;
    }
}

/* static */
bool
WordleHistory::Private::isValid(
    const HistoryEntry* aEntry)
{
    if (aEntry->iStartTime > 0 && aEntry->iEndTime > aEntry->iStartTime) {
        // There must be at least the answer and one guess
        for (int i = 0; i < 10; i++) {
            if (!aEntry->iState[i]) {
                return false;
            }
        }
        return true;
    }
    return false;
}

/* static */
bool
WordleHistory::Private::isWin(
    const HistoryEntry* aEntry)
{
    const ushort* lastAttempt = aEntry->iState +
        (WORDLE_WORD_LENGTH * WORDLE_MAX_ATTEMPTS);

    // If the game was finished with less than the maximum number of
    // attempts, it must be a win (or else it wouldn't finish).
    // Otherwise we only need to check the last attempt.
    return !lastAttempt[0] || !memcmp(lastAttempt, aEntry->iState,
        WORDLE_WORD_LENGTH * sizeof(aEntry->iState[0]));
}

/* static */
int
WordleHistory::Private::guessCount(
    const HistoryEntry* aEntry)
{
    for (int i = WORDLE_MAX_ATTEMPTS; i > 1; i--) {
        // The first 5 letters in iState is the answer
        if (aEntry->iState[WORDLE_WORD_LENGTH * i]) {
            return i;
        }
    }
    // There must have been at least one attempt
    return 1;
}

/* static */
QString
WordleHistory::Private::toString(
    const ushort* aChars,
    int aOffset,
    int aMaxCount)
{
    QString str;
    int n = 0;
    while (n < aMaxCount && aChars[aOffset + n]) n++;
    str.reserve(n);
    for (int i = 0; i < n; i++) {
        const ushort c(aChars[aOffset + i]);
        if (c) {
            str.append(QChar(c));
        } else {
            // Stop at first zero
            break;
        }
    }
    return str;
}

QVariant
WordleHistory::Private::data(
    int aRow,
    Role aRole)
{
    if (aRow >= 0 && aRow < iTotalCount) {
        const HistoryEntry* entry = iHistoryEntries + iTotalCount - aRow - 1;
        switch (aRole) {
        case WinRole:
            return isWin(entry);
        case AnswerRole:
            return toString(entry->iState, 0, WORDLE_WORD_LENGTH);
        case AttemptsRole:
            return toString(entry->iState, WORDLE_WORD_LENGTH,
                WORDLE_WORD_LENGTH * WORDLE_MAX_ATTEMPTS);
        case StartTimeRole:
            return QDateTime::fromMSecsSinceEpoch(entry->iStartTime,
                Qt::LocalTime);
        case EndTimeRole:
            return QDateTime::fromMSecsSinceEpoch(entry->iEndTime,
                Qt::LocalTime);
        case SecondsPlayedRole:
            return entry->iSecondsPlayed;
        }
    }
    return QVariant();
}

// ==========================================================================
// WordleHistory
// ==========================================================================

WordleHistory::WordleHistory(
    QObject* aParent) :
    QAbstractListModel(aParent),
    iPrivate(new Private(this))
{
}

WordleHistory::~WordleHistory()
{
    delete iPrivate;
}

QString
WordleHistory::language() const
{
    return iPrivate->iLanguage.getCode();
}

void
WordleHistory::setLanguage(
    const QString aLanguageCode)
{
    iPrivate->setLanguage(aLanguageCode);
    iPrivate->emitQueuedSignals();
}

int
WordleHistory::totalCount() const
{
    return iPrivate->iTotalCount;
}

int
WordleHistory::winCount() const
{
    return iPrivate->iWinCount;
}

int
WordleHistory::lastAttempts() const
{
    return iPrivate->iLastAttempts;
}

int
WordleHistory::totalSec() const
{
    return iPrivate->iTotalSec;
}

int
WordleHistory::minGameSec() const
{
    return iPrivate->iMinGameSec;
}

int
WordleHistory::currentStreak() const
{
    return iPrivate->iCurrentStreak;
}

int
WordleHistory::maxStreak() const
{
    return iPrivate->iMaxStreak;
}

QList<int>
WordleHistory::guessDistribution() const
{
    QList<int> list;
    list.reserve(WORDLE_MAX_ATTEMPTS);
    for (int i = 0; i < WORDLE_MAX_ATTEMPTS; i++) {
        list.append(iPrivate->iGuessCounts[i]);
    }
    return list;
}

QHash<int,QByteArray>
WordleHistory::roleNames() const
{
    QHash<int,QByteArray> roles;
    #define ROLE(X,x) roles.insert(Private::MODEL_ROLE(X), #x);
    MODEL_ROLES(ROLE)
    #undef ROLE
    return roles;
}

int
WordleHistory::rowCount(
    const QModelIndex&) const
{
    return totalCount();
}

QVariant
WordleHistory::data(
    const QModelIndex& aIndex,
    int aRole) const
{
    return iPrivate->data(aIndex.row(), (Private::Role) aRole);
}

void
WordleHistory::add(
    const QString aAnswer,
    const QStringList aAttempts,
    QDateTime aStartTime,
    QDateTime aEndTime,
    int aSecondsPlayed)
{
    iPrivate->add(aAnswer, aAttempts, aStartTime, aEndTime, aSecondsPlayed);
    iPrivate->emitQueuedSignals();
}

void
WordleHistory::clear()
{
    iPrivate->clear();
    iPrivate->emitQueuedSignals();
}
