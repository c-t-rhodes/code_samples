#!/bin/bash

date=`date +%Y%m%d`
echo $date

#create two files and put them in an archive
echo "This is a test file." > testfile1_bash_test.dat
echo -e "1\n2\n3\n4\n5\n6\n7\n8\n9\n10" > testfile2_bash_test.dat

echo -e "Creating archive ... \n"
tar -czf testfiles.tar.gz testfile*_bash_test.dat
rm testfile*_bash_test.dat

#re-extract
tar -xf testfiles.tar.gz

#parse through files, print contents and record count
echo "Parsing through tar file ..."
for i in `tar -tzf testfiles.tar.gz`
do
  echo "Found file $i"
  echo "-------------"
  cat $i
  rc=`wc -l < $i`
  echo -e "Record Count = $rc\n"
done

#can we find our files if we didn't know where they were?
echo "Can we find our testfile1.dat if we didn't know where it was?"
possible_files_count=`find / -name "testfile1_bash_test.dat" 2>/dev/null | wc -l`
my_file=""

if [ $possible_files_count == "1" ]
then
  echo "I think we found it!"
  my_file=`find / -name "testfile1_bash_test.dat" 2>/dev/null`
  echo -e "$my_file\n"
else
  echo "Found nothing or too many possibilities"
fi

#What about error checking?
echo "Error checking? What if want to know if it really removed that file?"
echo "Attempting to remove file (testfile1001001010100_alhweurhiwuhefiuwefh.dat) ... "
rm testfile1001001010100_alhweurhiwuhefiuwefh.dat

if [ $? -ne 0 ]
then
  echo "Attempted command did not succeed"
else
  echo "Success!"
fi

#file checking
echo "Let's check to see if it exists in the first place before attempting to remove it."

echo -e "\nChecking for file testfile2sidnfhksfhldskfjsdlfk.dat"
if [ -e testfile2sidnfhksfhldskfjsdlfk.dat ]
then
  echo "The file exists"
else
  echo "The file doesn't exist"
fi

echo -e "\nChecking for file testfile1_bash_test.dat"
if [ -e testfile1_bash_test.dat ]
then
  echo "The file exists"
else
  echo "The file doesn't exist"
fi

#let's do some unnecessary piping
echo -e "\nUnique record count of file2"
gzip -c testfile2_bash_test.dat | gzip -dc | sort | uniq | wc -l

echo -e "\nAdding some numbers to the end of testfile2_bash_test.dat"
echo -e "11\n12\n12\n12\n12" >> testfile2_bash_test.dat

echo -e "\nUnique record count of file2"
gzip -c testfile2_bash_test.dat | gzip -dc | sort | uniq | wc -l

#fun with sed
echo -e "\nSed some things..."
echo "Changing all numbers 11 to A, and 12 to B"
echo "BEFORE"
cat testfile2_bash_test.dat

#note: this is not something aix liked at all, atleast on our box
sed -i 's/11/A/g' testfile2_bash_test.dat
sed -i 's/12/B/g' testfile2_bash_test.dat

echo "AFTER"
cat testfile2_bash_test.dat

#lets grep for those Bs
echo -e "\nGrepping for those B records"
grep B testfile2_bash_test.dat