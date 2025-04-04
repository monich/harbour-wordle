/*
 * Copyright (C) 2025 Slava Monich <slava@monich.com>
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

#include "WordleSearchEngine.h"

#include "WordlePlatform.h"

#include "HarbourDebug.h"
#include "HarbourJson.h"

#include <QtCore/QDir>
#include <QtCore/QUrl>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

// ==========================================================================
// WordleSearchEngine::Private
// ==========================================================================

class WordleSearchEngine::Private
{
public:
    static const QString DIR;
    static const QString EXT;
    static const QString FILTER;

    static const QString INFO_KEY_NAME;
    static const QString INFO_KEY_QUERY_URL;

    static const QString DEFAULT_ID;
    static const QString DEFAULT_NAME;
    static const QString DEFAULT_QUERY_URL;
};

const QString WordleSearchEngine::Private::DIR("search");
const QString WordleSearchEngine::Private::EXT(".json");
const QString WordleSearchEngine::Private::FILTER("*.json");

const QString WordleSearchEngine::Private::INFO_KEY_NAME("name");
const QString WordleSearchEngine::Private::INFO_KEY_QUERY_URL("queryUrl");

const QString WordleSearchEngine::Private::DEFAULT_ID("default");
const QString WordleSearchEngine::Private::DEFAULT_NAME("Default");
const QString WordleSearchEngine::Private::DEFAULT_QUERY_URL("https://google.com/search?q=%1");

// ==========================================================================
// WordleSearchEngine
// ==========================================================================

WordleSearchEngine::WordleSearchEngine(
    QString aId) :
    iId(aId)
{
    const QFileInfo infoFile(PlatformApp::pathTo(Private::DIR +
        QDir::separator() + aId + Private::EXT).toLocalFile());
    QVariantMap info;
    HDEBUG(aId << infoFile.absoluteFilePath() << infoFile.exists());
    if (HarbourJson::load(infoFile.absoluteFilePath(), info)) {
        iName = info.value(Private::INFO_KEY_NAME).toString();
        iQueryUrl = info.value(Private::INFO_KEY_QUERY_URL).toString();
    }
}

WordleSearchEngine::WordleSearchEngine(
    QString aName,
    QString aQueryUrl) :
    iName(aName),
    iQueryUrl(aQueryUrl)
{}

bool
WordleSearchEngine::isValid() const
{
    return !iName.isEmpty() && iQueryUrl.contains(QStringLiteral("%1"));
}

bool
WordleSearchEngine::isDefault() const
{
    return iId.isEmpty() || iId == Private::DEFAULT_ID;
}

QString
WordleSearchEngine::id() const
{
    return iId;
}

QString
WordleSearchEngine::name() const
{
    return iName;
}

QString
WordleSearchEngine::queryUrl() const
{
    return iQueryUrl;
}

/* static  */
QList<WordleSearchEngine>
WordleSearchEngine::availableEngines()
{
    QList<WordleSearchEngine> list;
    const QDir dir(PlatformApp::pathTo(Private::DIR).toLocalFile());
    const QStringList filter(Private::FILTER);
    const QStringList files(dir.entryList(filter,
        QDir::Files | QDir::NoDotAndDotDot, QDir::Name));
    QStringList ids;
    int i, n = files.size();

    // Make the list of ids out of file names
    ids.reserve(n);
    for (i = 0; i < n; i++) {
        const QString f(files.at(i));

        if (f.endsWith(Private::EXT)) {
            const QString id(f.left(f.length() - Private::EXT.length()));

            // Make sure that the default entry goes first
            if (id == Private::DEFAULT_ID) {
                ids.insert(0, id);
            } else {
                ids.append(id);
            }
        }
    }

    // Build the list of valid engines
    n = ids.count();
    list.reserve(n);
    for (i = 0; i < n; i++) {
        WordleSearchEngine engine(ids.at(i));

        if (engine.isValid()) {
            list.append(engine);
        }
    }
    return list;
}

/* static  */
WordleSearchEngine
WordleSearchEngine::getEngine(
    QString aId)
{
    // This function always returns a valid object
    if (!aId.isEmpty() && aId != Private::DEFAULT_ID) {
        WordleSearchEngine engine(aId);

        if (engine.isValid()) {
            // Non-default engine
            return engine;
        }
    }

    // Try to load the default configuration from the filesystem
    WordleSearchEngine engine(Private::DEFAULT_ID);
    if (engine.isValid()) {
        return engine;
    }

    // Fall back to hardcoded defaults
    return WordleSearchEngine(Private::DEFAULT_NAME,
            Private::DEFAULT_QUERY_URL);
}
