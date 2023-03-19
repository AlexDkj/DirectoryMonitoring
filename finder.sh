#!/bin/bash

if [ -z $* ];
then
    echo "Give one or more arguments"
    exit 0
fi

cd ../out
ls *.out > tempor

num=()
count=0

for k in $*
do

    num+=(0)

done

while read i
do

  for d in $*
  do

    while read loc sum
    do

       case $loc in
            *.$d* )  let "num[$count] = num[$count] + $sum";;
       esac

    done < $i

    let "count = count + 1"

  done

  count=0

done < tempor

count=0

for l in $*
do

    echo -n $l
    echo " ${num[$count]}"
    let "count = count + 1"

done

rm tempor

