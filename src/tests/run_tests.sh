#!/bin/sh

TESTS_COUNT=1

cd ./tests

##### RUN TESTS ####

i=1
while [ $i -le $TESTS_COUNT ]
do
   ./../ifj16 test$i.code > test$i.output 2>/dev/null
   echo $? > test$i.exit_code
   EXIT="$(diff test$i.exit_code test$i.exit_code_ok)"
   TEST="$(diff test$i.output test$i.output_ok)"
   echo -n "TEST $i: "
   if [ -z "${TEST}" ];
   then
       echo -n "OK"
   else
       echo -n "FAIL"
   	   echo "${TEST}"
   fi

   echo -n ", exit code: "

   if [ -z "${EXIT}" ];
   then
       echo "OK"
   else
       echo "FAIL"
   	   echo "${EXIT}"
   fi

   i=`expr $i + 1`
done
