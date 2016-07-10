#!/bin/bash

# This is a short script for running 
# the test_hasher tool on the numers.log
# 'log file'. As the test_hasher is 
# mostly the same code as hasher, but 
# with just the difference of using an
# identity hash function, it will enable 
# better checking of the code output. 
# The numbers.log file has 10 lines, with 
# values from 0 to 9 on the lines. 

# Run with signing and leaves storing
./build/test_hasher -i example_logs/numbers.log --leaves

# Run with hash chain extraction for all lines
cp example_logs/numbers.log  example_logs/tmp_lines_for_numbers
./build/test_hasher -i example_logs/numbers.log --chain example_logs/tmp_lines_for_numbers
rm example_logs/tmp_lines_for_numbers

# Move the output to test_output directory
mkdir -p test_output
mv example_logs/numbers.log.* test_output/

# Echo that the script has completed
echo ''
echo 'Completed running tests'
echo ''


# Now print the numbers.log and the output files

for file in example_logs/numbers.log test_output/*.leaves test_output/*.signature test_output/*.hash_chain*
do 
	echo ${file}
	cat ${file}
	echo "" 
done


