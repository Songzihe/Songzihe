#!/bin/bash
#test file
# for the assign a

gcc concord1.c -o concord1 -std=c99 -g -Wall


echo 'Test 1'
./concord1 in01.txt | diff out01.txt -
echo 'Test 2'
./concord1 in02.txt | diff out02.txt -
echo 'Test 3'
./concord1 in03.txt -e english.txt| diff out03.txt -
echo 'Test 4'
./concord1 in04.txt -e english.txt| diff out04.txt -
echo 'Test 5'
./concord1 in05.txt -e english.txt| diff out05.txt -
echo 'Test 6'
./concord1 in06.txt -e english.txt| diff out06.txt -
echo 'Test 7'
./concord1 in07.txt -e english.txt| diff out07.txt -
echo 'Test 8'
./concord1 in08.txt -e latin.txt| diff out08.txt -
echo 'Test 9'
./concord1 -e latin.txt in09.txt | diff out09.txt -
echo 'Test 10'
./concord1 -e deutsch.txt in10.txt | diff out10.txt -
echo 'Done'
