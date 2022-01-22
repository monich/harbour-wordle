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

#include "WordleDefs.h"
#include "WordleLanguage.h"

#include "HarbourJson.h"
#include "HarbourDebug.h"

#include <sailfishapp.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextCodec>
#include <QVariantMap>
#include <QAtomicInt>
#include <QMap>

#include <string.h>
#include <stdlib.h>

// ==========================================================================
// WordleLanguage::Private
//
// This is a fairly heavy object, hence it's cached and ref-counted,
// and WordleLanguage is a shallow wrapper carrying a reference to
// WordleLanguage::Private
// ==========================================================================

class WordleLanguage::Private
{
public:
    static const QString DATA_DIR;
    static const QString INFO_FILE;
    static const QString WORDS_FILE;
    static const QString XWORDS_FILE;

    static const QString INFO_KEY_NAME;
    static const QString INFO_KEY_ALPHABET;
    static const QString INFO_KEY_ENCODING;
    static const QString INFO_KEY_KEYPAD;

    static QMap<QString, Private*> gLanguageMap;
    typedef int (*Comparator)(const void*, const void*);

    Private(const QString& aLanguageCode);
    ~Private();

    bool isValid();
    bool isAllowed(QString aWord);
    QString randomWord();
    const char* wordsData();
    const char* extWordsData();

    static int Compare1(const void *, const void *);

public:
    QAtomicInt iRef;
    const QString iLanguageCode;
    QString iName;
    QString iAlphabet;
    QStringList iKeypad;
    QFileInfo iWordsFileInfo;
    QFileInfo iExtWordsFileInfo;
    QTextCodec* iTextCodec;
    Comparator iCompare;
    int iCharSize;
    int iWordsCount;
    int iExtWordsCount;
    QFile iWordsFile;
    QFile iExtWordsFile;
    uchar* iWordsData;
    uchar* iExtWordsData;
};

const QString WordleLanguage::Private::DATA_DIR("data");
const QString WordleLanguage::Private::INFO_FILE("language.json");
const QString WordleLanguage::Private::WORDS_FILE("words");
const QString WordleLanguage::Private::XWORDS_FILE("xwords");

const QString WordleLanguage::Private::INFO_KEY_NAME("name");
const QString WordleLanguage::Private::INFO_KEY_ALPHABET("alphabet");
const QString WordleLanguage::Private::INFO_KEY_ENCODING("enconding");
const QString WordleLanguage::Private::INFO_KEY_KEYPAD("keypad");

QMap<QString, WordleLanguage::Private*> WordleLanguage::Private::gLanguageMap;

WordleLanguage::Private::Private(const QString& aLanguageCode) :
    iRef(1),
    iLanguageCode(aLanguageCode),
    iTextCodec(Q_NULLPTR),
    iCompare(Q_NULLPTR),
    iCharSize(0),
    iWordsCount(0),
    iExtWordsCount(0),
    iWordsData(Q_NULLPTR),
    iExtWordsData(Q_NULLPTR)
{
    QDir dataDir(SailfishApp::pathTo(DATA_DIR + QDir::separator() + aLanguageCode).toLocalFile());
    iExtWordsFileInfo = QFileInfo(dataDir, XWORDS_FILE);
    iWordsFileInfo = QFileInfo(dataDir, WORDS_FILE);
    QFileInfo infoFile(dataDir, INFO_FILE);
    HDEBUG(aLanguageCode << infoFile.absoluteFilePath() << infoFile.exists());
    QVariantMap info;
    if (HarbourJson::load(infoFile.absoluteFilePath(), info)) {
        iName = info.value(INFO_KEY_NAME).toString();
        iAlphabet = info.value(INFO_KEY_ALPHABET).toString().toLower();
        iKeypad = info.value(INFO_KEY_KEYPAD).toStringList();
        QByteArray enc(info.value(INFO_KEY_ENCODING).toString().toLatin1());
        if (enc.isEmpty()) {
            enc = QByteArray("ascii");
        }
        TODO("Support multibyte encodings");
        iCharSize = 1;
        iCompare = Compare1;
        iTextCodec = QTextCodec::codecForName(enc.constData());
        if (iTextCodec) {
            const int wordSize = WORDLE_WORD_LENGTH * iCharSize;
            iExtWordsCount = (int)(iExtWordsFileInfo.size()/wordSize);
            iWordsCount = (int)(iWordsFileInfo.size()/wordSize);
            iWordsFile.setFileName(iWordsFileInfo.absoluteFilePath());
            iExtWordsFile.setFileName(iExtWordsFileInfo.absoluteFilePath());
            if (isValid()) {
                gLanguageMap.insert(iLanguageCode, this);
                HDEBUG(iLanguageCode << "loaded" << iWordsCount << iExtWordsCount);
            }
        } else {
            HWARN("Not text codec for" << enc);
        }
    }
}

WordleLanguage::Private::~Private()
{
    if (gLanguageMap.value(iLanguageCode) == this) {
        gLanguageMap.remove(iLanguageCode);
        HDEBUG("done with" << iLanguageCode);
    }
    if (iWordsData) {
        iWordsFile.unmap(iWordsData);
    }
    if (iExtWordsData) {
        iExtWordsFile.unmap(iExtWordsData);
    }
}

bool WordleLanguage::Private::isValid()
{
    return iTextCodec && !iKeypad.isEmpty() && !iAlphabet.isEmpty() && iWordsCount > 0;
}

const char* WordleLanguage::Private::wordsData()
{
    if (!iWordsData) {
        if (iWordsFile.open(QFile::ReadOnly)) {
            iWordsData = iWordsFile.map(0, iWordsFile.size());
        }
        if (iWordsData) {
            HDEBUG("Opened" << qPrintable(iWordsFile.fileName()));
        } else {
            HWARN("Failed to map" << qPrintable(iWordsFile.fileName()));
        }
    }
    return (const char*) iWordsData;
}

const char* WordleLanguage::Private::extWordsData()
{
    if (iExtWordsCount && !iExtWordsData) {
        if (iExtWordsFile.open(QFile::ReadOnly)) {
            iExtWordsData = iExtWordsFile.map(0, iExtWordsFile.size());
        }
        if (iExtWordsData) {
            HDEBUG("Opened" << qPrintable(iExtWordsFile.fileName()));
        } else {
            HWARN("Failed to map" << qPrintable(iExtWordsFile.fileName()));
        }
    }
    return (const char*) iExtWordsData;
}

QString WordleLanguage::Private::randomWord()
{
    const char* words = wordsData();
    if (words) {
        const int wordSize = WORDLE_WORD_LENGTH * iCharSize;
        const int i = qrand() % iWordsCount;
        const char* wordBytes = words + (i * wordSize);
        const QString word(iTextCodec->toUnicode(QByteArray(wordBytes, wordSize)));
        HDEBUG(word);
        return word;
    }
    return QString();
}

bool WordleLanguage::Private::isAllowed(QString aWord)
{
    if (aWord.length() == WORDLE_WORD_LENGTH) {
        const QByteArray wordBytes(iTextCodec->fromUnicode(aWord));
        const char* word = wordBytes.constData();
        const int wordSize = WORDLE_WORD_LENGTH * iCharSize;

        HASSERT(wordBytes.length() == wordSize);

        // First try the main dictionary
        const char* words = wordsData();
        if (words && bsearch(word, words, iWordsCount, wordSize, iCompare)) {
            HDEBUG(aWord << "is ok");
            return true;
        }

        HDEBUG(aWord << "is not in the main dictionary");

        // Then the extended one
        words = extWordsData();
        if (words && bsearch(word, words, iExtWordsCount, wordSize, iCompare)) {
            HDEBUG(aWord << "is ok");
            return true;
        }
    }
    HDEBUG(aWord << "is not allowed");
    return false;
}

// Comparators for single-byte encoding
int WordleLanguage::Private::Compare1(const void* aData1, const void* aData2)
{
    return memcmp(aData1, aData2, WORDLE_WORD_LENGTH);
}

// ==========================================================================
// WordleLanguage
// ==========================================================================

WordleLanguage::WordleLanguage(const QString& aLangCode) :
    iPrivate(Private::gLanguageMap.value(aLangCode))
{
    if (iPrivate) {
        iPrivate->iRef.ref();
    } else if (!aLangCode.isEmpty()) {
        iPrivate = new Private(aLangCode);
        if (!iPrivate->isValid()) {
            delete iPrivate;
            iPrivate = Q_NULLPTR;
        }
    }
}

WordleLanguage::WordleLanguage(const WordleLanguage& aLang) :
    iPrivate(aLang.iPrivate)
{
    if (iPrivate) {
        iPrivate->iRef.ref();
    }
}

WordleLanguage::WordleLanguage() :
    iPrivate(Q_NULLPTR)
{
}

WordleLanguage::~WordleLanguage()
{
    if (iPrivate && !iPrivate->iRef.deref()) {
        delete iPrivate;
    }
}

WordleLanguage& WordleLanguage::operator = (const WordleLanguage& aLang)
{
    if (iPrivate != aLang.iPrivate) {
        if (iPrivate && !iPrivate->iRef.deref()) {
            delete iPrivate;
        }
        iPrivate = aLang.iPrivate;
        if (iPrivate) {
            iPrivate->iRef.ref();
        }
    }
    return *this;
}

bool WordleLanguage::isValid() const
{
    return iPrivate != Q_NULLPTR;
}

const QString WordleLanguage::getCode() const
{
    return iPrivate ? iPrivate->iLanguageCode : QString();
}

const QString WordleLanguage::getName() const
{
    return iPrivate ? iPrivate->iName : QString();
}

const QString WordleLanguage::getAlphabet() const
{
    return iPrivate ? iPrivate->iAlphabet : QString();
}

const QStringList WordleLanguage::getKeypad() const
{
    return iPrivate ? iPrivate->iKeypad : QStringList();
}

QString WordleLanguage::randomWord() const
{
    return iPrivate ? iPrivate->randomWord() : QString();
}

bool WordleLanguage::isAllowed(QString aWord) const
{
    return iPrivate && iPrivate->isAllowed(aWord);
}

QList<WordleLanguage> WordleLanguage::availableLangiages()
{
    QList<WordleLanguage> languages;

    QDir dataDir(SailfishApp::pathTo(Private::DATA_DIR).toLocalFile());
    QStringList dirs(dataDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name));
    const int n = dirs.count();
    for (int i = 0; i < n; i++) {
        WordleLanguage language(dirs.at(i));
        if (language.isValid()) {
            languages.append(language);
        }
    }
    return languages;
}
