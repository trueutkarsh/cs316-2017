make clean
make
rm -rf testcases16
mkdir testcases16
rm -rf testcases/*.spim
cp testcases/* testcases16/
l=$(ls testcases/ | grep -P ".c$" | wc -l)
for i in `seq 1 $l`;
do
	./sclp "testcases/test$i.c"
	./sclp16 "testcases16/test$i.c"
	diff "testcases/test$i.c.spim" "testcases16/test$i.c.spim" &> /dev/null
	if [ $? = 0 ];
	then
		echo "Test Case $i alright"
	else
		echo "OOPS! wrong for test case $i"
	fi
done

