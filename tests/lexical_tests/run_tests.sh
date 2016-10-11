#!/bin/sh

TESTS_COUNT=8

cd ../../src
make -s

cp main_lexical_tests ../tests/lexical_tests
cd ../tests/lexical_tests

##### RUN TESTS ####


echo "Running lexical analysis tests...\n"

i=1
while [ $i -le $TESTS_COUNT ]
do
   ./main_lexical_tests test$i.ifj > test$i.output
   TEST="$(diff test$i.output test$i.output.ok)"
   echo -n "TEST $i: "
   if [ -z "${TEST}" ];
   then
       echo "OK"
   else
       echo "FAIL"
   	echo "${TEST}"
   fi

   rm -f test$i.output

   i=`expr $i + 1`
done

#######################

rm -f main_lexical_tests
rm -f ../../src/main_lexical_tests
