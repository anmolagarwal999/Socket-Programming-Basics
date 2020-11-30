#!/bin/bash
filename='debug4.txt'
n=1
echo "here"
while read line; do
    # reading each line
    echo "inside"
    echo "Line No. $n : $line"
    # ./a $line
    # ./g  $line
   # ./part1 < $line

    out1="./${line}"
    out2="./client_folder/${line}"


    echo "out1 is ${out1}"
    echo "out2 is ${out2}"



    diff -w $out1 $out2 || break


    n=$((n + 1))
    echo '------------------------------'
done <$filename

echo "STUFF DONE "
