#!/bin/sh

# Loop through tests in UMTESTS

# For each test, run it (with input if exists) and store output (to diff with
# expected output if exists)

# Make the tests
tests_folder="um-lab"
cd $tests_folder
make
./writetests
cd ..

# Loop through all the tests
filename="UMTESTS"
while IFS= read -r line; do
        # Where to output and input from if no in/output
        expected_out="/dev/null"
        expected_in="/dev/null"
        
        # from lab, get without .um
        testName=$(echo "$line" | sed -E 's/(.*).um/\1/') 
        
        # If there's a .0 use as input
        if [ -f "$tests_folder/$testName.0" ]; then
                expected_in="$tests_folder/$testName.0"
        fi

        # If there's a .1 use as output
        if [ -f "$tests_folder/$testName.1" ]; then
                expected_out="$tests_folder/$testName.1"
        fi
        
        # Run um and diff with output. If there's a difference, we failed
        echo "Test $line:"
        ./um $tests_folder/$line < $expected_in > temp_test_output.out
        if [ "$expected_out" != "/dev/null" ]; then
                # cat $expected_out
                $(diff temp_test_output.out "$expected_out" > /dev/null)
                if [ $(echo $?) != 0 ]; then
                        echo "----"
                        echo "$line test FAILED"
                        echo "----"
                fi
        fi

        # Clean up
        rm temp_test_output.out

done < "$filename"


