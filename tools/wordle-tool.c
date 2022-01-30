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

#include <glib.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define RET_OK 0
#define RET_ERR 1
#define RET_CMDLINE 2

#define WORDS_FILE "words"
#define XWORDS_FILE "xwords"
#define UTF8_ENC "utf8"
#define DEFAULT_INPUT_ENC UTF8_ENC
#define WORD_SIZE 5

static gboolean be_verbose = FALSE;

static
void
errmsg(
    const char* format,
    ...) G_GNUC_PRINTF(1,2);

static
void
output(
    const char* format,
    ...) G_GNUC_PRINTF(1,2);

static
void
verbose(
    const char* format,
    ...) G_GNUC_PRINTF(1,2);

static
void
errmsg(
    const char* format,
    ...)
{
    va_list va;

    va_start(va, format);
    vfprintf(stderr, format, va);
    va_end(va);
}

static
void
output(
    const char* format,
    ...)
{
    va_list va;

    va_start(va, format);
    vfprintf(stdout, format, va);
    va_end(va);
}

static
void
verbose(
    const char* format,
    ...)
{
    if (be_verbose) {
        va_list va;

        va_start(va, format);
        vfprintf(stdout, format, va);
        va_end(va);
    }
}

static
int
compare(
    const void* a,
    const void* b)
{
    return memcmp(a, b, WORD_SIZE);
}

static
gulong
remove_dups(
    char* words,
    gulong n)
{
    const gulong n1 = n;
    gulong i;

    qsort(words, n, WORD_SIZE, compare);

    /* Remove duplicates */
    for (i = 0; i < n - 1;) {
        char* word = words + i * WORD_SIZE;
        char* next_word = word + WORD_SIZE;

        if (!compare(word, next_word)) {
            verbose("Duplicate word #%lu\n", i + 1);
            memmove(word, next_word, WORD_SIZE * (n - i - 1));
            n--;
        } else {
            i++;
        }
    }

    if (n1 > n) {
        output("Removed %lu word(s)\n", n1 - n);
    }

    return n;
}

static
GBytes*
load_list(
    const char* file,
    const char* in_enc,
    const char* out_enc)
{
    GBytes* out = NULL;
    GError* error = NULL;
    char* in = NULL;
    gsize in_len = 0;

    if (g_file_get_contents(file, &in, &in_len, &error)) {
        const char* utf8 = in;
        gsize utf8_len = in_len;
        char* tmp_utf8 = NULL;

        if (in_enc && g_ascii_strcasecmp(in_enc, UTF8_ENC)) {
            utf8 = tmp_utf8 = g_convert(in, in_len, UTF8_ENC, in_enc, NULL,
                &utf8_len, &error);
            if (utf8) {
                verbose("Converted %s %s (%lu bytes) => %s (%lu bytes)\n",
                    file, in_enc, (gulong) in_len, UTF8_ENC, (gulong)
                    utf8_len);
            }
        }

        if (utf8) {
            GByteArray* buf = g_byte_array_sized_new(utf8_len);
            const char* ptr = utf8;
            char* words;
            gsize i, words_len, converted = 0;

            /* Remove spaces */
            for (i = 0; i < utf8_len; i++) {
                const guint8 b = *ptr++;

                if (!isspace(b)) {
                    g_byte_array_append(buf, &b, 1);
                }
            }

            utf8 = (const char*) buf->data;

            if (utf8_len > buf->len) {
                verbose("Removed %lu bytes from " UTF8_ENC " buffer\n",
                    (gulong) (utf8_len - buf->len));
                utf8_len = buf->len;
            }

            /* Convert to output single-byte encoding */
            words = g_convert(utf8, utf8_len, out_enc, UTF8_ENC, &converted,
                &words_len, &error);

            /* Done with UTF-8 */
            g_byte_array_free(buf, TRUE);

            if (words) {
                verbose("Converted %lu bytes of " UTF8_ENC
                    " => %lu bytes of %s\n", (gulong) utf8_len,
                    (gulong) words_len, out_enc);

                /* Check the length */
                if (words_len % WORD_SIZE) {
                    errmsg("Invalid number of letters %lu\n", (gulong)
                        words_len);
                    g_free(words);
                } else {
                    const gulong n = remove_dups(words, words_len / WORD_SIZE);

                    output("%lu words\n", n);
                    out = g_bytes_new_take(words, n * WORD_SIZE);
                }
            }
        }

        g_free(tmp_utf8);
        g_free(in);
    }
    if (error) {
        errmsg("%s\n", error->message);
        g_error_free(error);
    }
    return out;
}

static
GBytes*
load_words(
    const char* file)
{
    GBytes* out = NULL;
    char* words = NULL;
    gsize len = 0;

    if (g_file_get_contents(file, &words, &len, NULL)) {
        /* Check the length */
        if (len % WORD_SIZE) {
            errmsg("Invalid number of letters %lu\n", (gulong) len);
            g_free(words);
        } else {
            gulong n = remove_dups(words, len / WORD_SIZE);

            verbose("Loaded %lu %s\n", n, file);
            out = g_bytes_new_take(words, n * WORD_SIZE);
        }
    } else {
        out = g_bytes_new(NULL, 0);
    }
    return out;
}

static
int
save_words(
    const char* file,
    GBytes* words,
    gboolean add)
{
    gsize size;
    const void* bytes = g_bytes_get_data(words, &size);

    if (size) {
        GError* error = NULL;
        char* fdata = NULL;
        gsize fsize = 0;

        if (g_file_get_contents(file, &fdata, &fsize, NULL)) {
            /* Check the length */
            if (fsize % WORD_SIZE) {
                errmsg("Invalid number of letters %lu in %s\n", (gulong)
                    fsize, file);
                g_free(fdata);
                return RET_ERR;
            } else {
                if (add) {
                    /* Add new data, sort, remove dups */
                    fdata = g_realloc(fdata, fsize + size);
                    memcpy(fdata + fsize, bytes, size);
                    fsize = remove_dups(fdata, (fsize + size) / WORD_SIZE) *
                        WORD_SIZE;
                } else {
                    /* Just sort */
                    qsort(fdata, fsize / WORD_SIZE, WORD_SIZE, compare);
                }
                /* Has anything changed? */
                if (fsize == size && !memcmp(bytes, fdata, size)) {
                    verbose("File %s is unchanged\n", file);
                    g_free(fdata);
                    return RET_OK;
                }
            }
        }

        g_free(fdata);
        if (g_file_set_contents(file, bytes, size, &error)) {
            output("Wrote %s\n", file);
            return RET_OK;
        } else {
            errmsg("%s\n", error->message);
            g_error_free(error);
            return RET_ERR;
        }
    } else {
        output("Nothing to %s to %s\n", add ? "add" : "save", file);
        return RET_OK;
    }
}

static
GBytes*
remove_dups2(
    GBytes* xwords,
    GBytes* words)
{
    /* Makes sure that words don't appear in xwords */
    gsize words_size, xwords_size;
    const char* words_bytes = g_bytes_get_data(words, &words_size);
    const char* xwords_bytes = g_bytes_get_data(xwords, &xwords_size);
    char* xbuf = g_malloc(xwords_size);
    const gsize nwords = words_size/WORD_SIZE;
    gsize i, n = xwords_size/WORD_SIZE;

    memcpy(xbuf, xwords_bytes, xwords_size);
    for (i = 0; i < n;) {
        char* xword = xbuf + i * WORD_SIZE;

        if (bsearch(xword, words_bytes, nwords, WORD_SIZE, compare)) {
            verbose("Duplicate word #%lu\n", i + 1);
            memmove(xword, xword + WORD_SIZE, WORD_SIZE * (n - i - 1));
            n--;
        } else {
            i++;
        }
    }

    if (n < xwords_size/ WORD_SIZE) {
        output("Removed %lu xword(s)\n", (gulong)(xwords_size/WORD_SIZE - n));
        g_bytes_unref(xwords);
        return g_bytes_new_take(xbuf, n * WORD_SIZE);
    } else {
        g_free(xbuf);
        return xwords;
    }
}

static
int
run(
    const char* file,
    const char* input_enc,
    const char* output_enc,
    gboolean add,
    gboolean xwords)
{
    int ret = RET_ERR;

    if (file) {
        GBytes* in = load_list(file, input_enc, output_enc);

        if (in) {
            const gsize size = g_bytes_get_size(in);

            if (!size) {
                output("Nothing to %s\n", add ? "add" : "save");
                ret = RET_OK;
            } else if (xwords) {
                GBytes* words = load_words(WORDS_FILE);

                in = remove_dups2(in, words);
                ret = save_words(XWORDS_FILE, in, add);
                g_bytes_unref(words);
            } else {
                ret = save_words(WORDS_FILE, in, add);
            }
            g_bytes_unref(in);
        }
    } else {
        GBytes* words = load_words(WORDS_FILE);

        if (xwords) {
            GBytes* xwords = remove_dups2(load_words(XWORDS_FILE), words);

            ret = save_words(XWORDS_FILE, xwords, FALSE);
            g_bytes_unref(xwords);
        } else {
            ret = save_words(WORDS_FILE, words, FALSE);
        }
        g_bytes_unref(words);
    }
    return ret;
}

int
main(
    int argc,
    char* argv[])
{
    int ret = RET_CMDLINE;
    gboolean xwords = FALSE;
    gboolean add = FALSE;
    char* input_enc = NULL;
    GError* error = NULL;
    GOptionContext* options;
    GOptionEntry entries[] = {
        { "verbose", 'v', 0, G_OPTION_ARG_NONE, &be_verbose,
          "Enable verbose output", NULL },
        { "add", 'a', 0, G_OPTION_ARG_NONE, &add,
          "Add words to the dictionary (rather than replace)", NULL },
        { "xwords", 'x', 0, G_OPTION_ARG_NONE, &xwords,
          "Write 'xwords' file rather than 'words'", NULL },
        { "input", 'i', 0, G_OPTION_ARG_STRING, &input_enc,
          "Input encoding [" DEFAULT_INPUT_ENC "]", "ENC" },
        { NULL }
    };

    options = g_option_context_new("[LIST] ENC");
    g_option_context_add_main_entries(options, entries, NULL);
    g_option_context_set_summary(options,
        "The input file LIST contain space-separated words.\n"
        "Writes 'words' file containing sorted words in the chosen\n"
        "single-byte encoding ENC with spaces and duplicates removed.\n\n"
        "Without the input file, it checks the output file for consistency.");

    if (g_option_context_parse(options, &argc, &argv, &error)) {
        if (argc == 3) {
            ret = run(argv[1], input_enc, argv[2], add, xwords);
        } else if (argc == 2) {
            if (add) {
                errmsg("-a (--add) requires the input file\n");
            } else {
                ret = run(NULL, input_enc, argv[1], add, xwords);
            }
        } else {
            char* help = g_option_context_get_help(options, TRUE, NULL);

            errmsg("%s", help);
            g_free(help);
        }
    } else {
        errmsg("%s\n", error->message);
        g_error_free(error);
    }

    g_free(input_enc);
    g_option_context_free(options);
    return ret;
}

/*
 * Local Variables:
 * mode: C
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
