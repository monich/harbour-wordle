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

#ifndef WORDLE_TYPES_H
#define WORDLE_TYPES_H

#include <QtGlobal>

#if QT_VERSION < 0x050000
#define QStringLiteral(s) QLatin1String(s)
#endif

#ifndef Q_NULLPTR
#define Q_NULLPTR NULL
#endif // Q_NULLPTR

#ifndef Q_DECL_OVERRIDE
#define Q_DECL_OVERRIDE
#endif // Q_DECL_OVERRIDE

#ifndef Q_STATIC_ASSERT
template <bool Test> class QStaticAssertFailure;
template <> class QStaticAssertFailure<true> {};
#define Q_STATIC_ASSERT_PRIVATE_JOIN(A, B) Q_STATIC_ASSERT_PRIVATE_JOIN_IMPL(A, B)
#define Q_STATIC_ASSERT_PRIVATE_JOIN_IMPL(A, B) A ## B
#define Q_STATIC_ASSERT(Condition) \
    enum {Q_STATIC_ASSERT_PRIVATE_JOIN(q_static_assert_result, __LINE__) = sizeof(QStaticAssertFailure<!!(Condition)>)}
#endif // Q_STATIC_ASSERT

#endif // WORDLE_TYPES_H
