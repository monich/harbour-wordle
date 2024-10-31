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

#include "WordleLanguageModel.h"

#define MODEL_ROLES_(first,role,last) \
    first(LanguageCode,languageCode) \
    role(LanguageName,languageName) \
    role(Name,name) \
    role(WordCount,wordCount) \
    last(ExtraWordCount,extraWordCount)

#define MODEL_ROLES(role) \
    MODEL_ROLES_(role,role,role)

#define MODEL_ROLE(X) X##Role

enum WordleLanguageModelRole {
    #define FIRST(X,x) FirstRole = Qt::UserRole, MODEL_ROLE(X) = FirstRole,
    #define ROLE(X,x) MODEL_ROLE(X),
    #define LAST(X,x) MODEL_ROLE(X), LastRole = MODEL_ROLE(X)
    MODEL_ROLES_(FIRST,ROLE,LAST)
    #undef FIRST
    #undef ROLE
    #undef LAST
};

WordleLanguageModel::WordleLanguageModel(
    QObject* aParent) :
    QAbstractListModel(aParent),
    iLanguages(WordleLanguage::availableLanguages())
{
#if QT_VERSION < 0x050000
    setRoleNames(roleNames());
#endif
}

QHash<int,QByteArray>
WordleLanguageModel::roleNames() const
{
    QHash<int,QByteArray> roles;
    #define ROLE(X,x) roles.insert(MODEL_ROLE(X), #x);
    MODEL_ROLES(ROLE)
    #undef ROLE
    return roles;
}

int
WordleLanguageModel::rowCount(
    const QModelIndex&) const
{
    return iLanguages.count();
}

QVariant
WordleLanguageModel::data(
    const QModelIndex& aIndex,
    int aRole) const
{
    const int i = aIndex.row();
    if (i >= 0 && i < iLanguages.count()) {
        const WordleLanguage& language(iLanguages.at(i));
        switch ((WordleLanguageModelRole)aRole) {
        case NameRole: // fallthrough
        case LanguageNameRole: return language.getName();
        case LanguageCodeRole: return language.getCode();
        case WordCountRole: return language.getWordCount();
        case ExtraWordCountRole: return language.getExtraWordCount();
        }
    }
    return QVariant();
}

QString
WordleLanguageModel::languageAt(
    int aRow)
{
    return (aRow >= 0 && aRow < iLanguages.count()) ?
        iLanguages.at(aRow).getCode() : QString();
}

int
WordleLanguageModel::indexOf(
    QString aCode)
{
    const int n = iLanguages.count();
    for (int i = 0; i < n; i++) {
        if (!iLanguages.at(i).getCode().compare(aCode, Qt::CaseInsensitive)) {
            return i;
        }
    }
    return -1;
}
