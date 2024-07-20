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

#include <glib.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define RET_OK 0
#define RET_ERR 1
#define RET_CMDLINE 2

#define UTF8_ENC "utf8"
#define DEFAULT_INPUT_ENC "windows-1252"
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
sort_proc(
    gconstpointer a,
    gconstpointer b,
    gpointer table)
{
    
    gconstpointer key1 = GINT_TO_POINTER(*(char*)a);
    gconstpointer key2 = GINT_TO_POINTER(*(char*)b);
    const gint v1 = GPOINTER_TO_INT(g_hash_table_lookup(table, key1));
    const gint v2 = GPOINTER_TO_INT(g_hash_table_lookup(table, key2));

    return v2 - v1;
}

static
void
print_stats(
    GHashTable* t,
    const char* in_enc,
    const char* out_enc)
{
    gsize i;
    float total = 0.f;
    const gsize n = g_hash_table_size(t);
    GByteArray* letters = g_byte_array_sized_new(n);
    GHashTableIter it;
    gpointer key;

    g_hash_table_iter_init(&it, t);
    while (g_hash_table_iter_next(&it, &key, NULL)) {
        const char letter = GPOINTER_TO_INT(key);

        g_byte_array_append(letters, (gconstpointer) &letter, 1);
        total += GPOINTER_TO_UINT(g_hash_table_lookup(t, key));
    }

    g_byte_array_sort_with_data(letters, sort_proc, t);

    for (i = 0; i < n; i++) {
        const char letter = letters->data[i];
        gconstpointer key = GINT_TO_POINTER(letter);
        const int count = GPOINTER_TO_UINT(g_hash_table_lookup(t, key));
        char* utf8 = g_convert(&letter, 1, out_enc, in_enc, NULL, NULL, NULL);

        output("%s %d %.2f%%\n", utf8, count, 100.f * count/total);
        g_free(utf8);
    }

    g_byte_array_free(letters, TRUE);
}

static
void
collect_stats(
    GHashTable* t,
    const char* letters,
    gsize len)
{
    gsize i;

    for (i = 0; i < len; i++) {
        const gpointer key = GINT_TO_POINTER(letters[i]);
        const int value = GPOINTER_TO_UINT(g_hash_table_lookup(t, key));

        g_hash_table_insert(t, key, GUINT_TO_POINTER(value + 1));
    }
}

static
int
process_file(
    GHashTable* t,
    const char* file)
{
    int ret = RET_ERR;
    GError* error = NULL;
    char* words = NULL;
    gsize len = 0;

    if (g_file_get_contents(file, &words, &len, &error)) {
        /* Check the length */
        if (len % WORD_SIZE) {
            errmsg("%s has invalid length %" G_GSIZE_FORMAT "\n", file, len);
        } else {
            verbose("Loaded %" G_GSIZE_FORMAT " words from %s\n",
                (len / WORD_SIZE), file);
            collect_stats(t, words, len);
            ret = RET_OK;
        }
        g_free(words);
    } else {
        errmsg("%s\n", error->message);
        g_error_free(error);
    }
    return ret;
}

int
main(
    int argc,
    char* argv[])
{
    int ret = RET_CMDLINE;
    char* enc = NULL;
    char* dir = NULL;
    GError* error = NULL;
    GOptionContext* options;
    GOptionEntry entries[] = {
        { "verbose", 'v', 0, G_OPTION_ARG_NONE, &be_verbose,
          "Enable verbose output", NULL },
        { "encoding", 'e', 0, G_OPTION_ARG_STRING, &enc,
          "Encoding [" DEFAULT_INPUT_ENC "]", "ENC" },
        { NULL }
    };

    options = g_option_context_new("FILE");
    g_option_context_add_main_entries(options, entries, NULL);
    g_option_context_set_summary(options, "Collects letter frequency stats.");

    if (g_option_context_parse(options, &argc, &argv, &error)) {
        if (argc > 1) {
            int i;
            GHashTable* t = g_hash_table_new(g_direct_hash, g_direct_equal);

            for (i = 1; i < argc; i++) {
                if ((ret = process_file(t, argv[i])) != RET_OK) {
                    break;
                }
            }

            if (ret == RET_OK) {
                print_stats(t, enc ? enc : DEFAULT_INPUT_ENC, UTF8_ENC);
            }

            g_hash_table_destroy(t);
        } else {
            char* help = g_option_context_get_help(options, TRUE, NULL);

            errmsg("%s", help);
            g_free(help);
        }
    } else {
        errmsg("%s\n", error->message);
        g_error_free(error);
    }

    g_free(dir);
    g_free(enc);
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
