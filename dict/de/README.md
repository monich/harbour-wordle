## Notes about working with dictionaries.

* abolish all plurals?
* abolish names altogether? 


## Questionable entries

 * Autun (Geological Era)

## Mising
 

## aspell 

### dump and select all words (and forms) for a lang (de).

aspell -d de dump master | aspell -l de expand > words.de.aspell.txt

### place all variants on a line of there own
sed -E -e 's/\s+/\n/g'  > words.de.list.aspell.txt

###  select only those which are 5 chars long
awk -v f=1 '$f ~ /^[[:alnum:]]{5}$/ { print $f }' words.de.list.aspell.txt > words.de.aspell.5.txt


## Using dict(2) which allows you to sort out not found

```
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
```

