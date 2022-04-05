#!/usr/bin/env bash

DICT_LANGUAGE="fd-por-eng"

function test_word() {
  dict -C -d "$2" "$1"
  EXISTS=$?
  if [ "$EXISTS" -eq 0 ]; then
    echo "word '$1' exists"
    echo "$1" >> "./$3"
  else
    echo "word '$1' does not exist"
    echo "$1" >> "./$4"
  fi
#  sleep 1
}

function test_file() {
  > "./$3"
  > "./$4"
  for i in `cat $1`; do
    test_word "$i" "$2" "$3" "$4"
  done
}

function generate_list() {
  aspell -d pt_BR dump master | sed -e 's:/.*::g' -e '/-/d' -e '/[A-Z]/d' -e '/^.\{1,4\}$/d' -e '/.\{6\}/d' > "./$1"
}

generate_list allwords.txt
test_file allwords.txt "$DICT_LANGUAGE" words.txt xwords.txt
