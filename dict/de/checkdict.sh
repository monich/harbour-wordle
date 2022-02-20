#!/usr/bin/env bash

file=$1

rejects() {
  test=$(dict -C  -d german-english "$1")
  # local return will be 20 for not found
  local t1=$?;
  if [ $t1 == 20 ] 
  then 
    #printf '%d\n' "$t1"
    echo "$1" >> rejected.txt
  fi 
}

confirms() {
  test=$(dict -C -d german-english "$1")
  # local return will be 0 for found
  local t1=$?;
  if [ $t1 == 0 ] 
  then 
    #printf '%d\n' "$t1"
    echo "$1" >> confirmed.txt
  fi 
}
for word in `cat $file`
  do
    rejects $word
    confirms $word
done
