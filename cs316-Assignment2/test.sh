#!/bin/bash

make clean
make
rm -rf testcases16
mkdir testcases16
rm -rf testcases/*.spim
cp testcases/* testcases16/
FILES=./testcases/*.c
# l=$(ls testcases/ | grep -P ".c$" | wc -l)
for i in $FILES
do
	./sclp "$i"
	./sclp16 "testcases16/$i"
	diff "testcases/$i.spim" "testcases16/$i.spim" &> /dev/null
	if [ $? = 0 ];
	then
		echo "Test Case $i alright"
	else
		echo "OOPS! wrong for test case $i"
	fi
	# echo $i
done

