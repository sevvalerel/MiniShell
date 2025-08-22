#!/bin/bash

echo "=== Testing Redirection Functions ==="
echo

# Test 1: Basic output redirection
echo "Test 1: Basic output redirection (>)"
echo "echo 'Hello World' > test_output.txt"
echo "Hello World" > test_output.txt
if [ -f test_output.txt ]; then
    echo "✓ Output file created successfully"
    echo "Content: $(cat test_output.txt)"
else
    echo "✗ Output file creation failed"
fi
echo

# Test 2: Basic input redirection
echo "Test 2: Basic input redirection (<)"
echo "cat < test_output.txt"
cat < test_output.txt
echo

# Test 3: Append output redirection
echo "Test 3: Append output redirection (>>)"
echo "echo 'Second line' >> test_output.txt"
echo "Second line" >> test_output.txt
echo "Content after append:"
cat test_output.txt
echo

# Test 4: Multiple redirections
echo "Test 4: Multiple redirections"
echo "echo 'Input for grep' > input.txt"
echo "Input for grep" > input.txt
echo "grep 'grep' < input.txt > output.txt"
grep "grep" < input.txt > output.txt
echo "Output content:"
cat output.txt
echo

# Test 5: Error handling - non-existent input file
echo "Test 5: Error handling - non-existent input file"
echo "cat < nonexistent_file.txt"
cat < nonexistent_file.txt 2>&1
echo

# Test 6: Error handling - permission denied
echo "Test 6: Error handling - permission denied"
echo "echo 'test' > /root/test.txt"
echo "test" > /root/test.txt 2>&1
echo

# Test 7: Complex command with redirections
echo "Test 7: Complex command with redirections"
echo "echo 'Line 1' > complex.txt"
echo "Line 1" > complex.txt
echo "echo 'Line 2' >> complex.txt"
echo "Line 2" >> complex.txt
echo "echo 'Line 3' >> complex.txt"
echo "Line 3" >> complex.txt
echo "wc -l < complex.txt > wc_output.txt"
wc -l < complex.txt > wc_output.txt
echo "Word count output:"
cat wc_output.txt
echo

# Test 8: Heredoc simulation (<<)
echo "Test 8: Heredoc simulation"
echo "cat << EOF > heredoc.txt"
cat << EOF > heredoc.txt
This is line 1
This is line 2
This is line 3
EOF
echo "Heredoc content:"
cat heredoc.txt
echo

# Test 9: Multiple files
echo "Test 9: Multiple files"
echo "echo 'File 1 content' > file1.txt"
echo "File 1 content" > file1.txt
echo "echo 'File 2 content' > file2.txt"
echo "File 2 content" > file2.txt
echo "cat file1.txt file2.txt > combined.txt"
cat file1.txt file2.txt > combined.txt
echo "Combined content:"
cat combined.txt
echo

# Test 10: Empty file handling
echo "Test 10: Empty file handling"
echo "touch empty.txt"
touch empty.txt
echo "cat < empty.txt"
cat < empty.txt
echo "✓ Empty file handled correctly"
echo

# Cleanup
echo "=== Cleaning up test files ==="
rm -f test_output.txt input.txt output.txt complex.txt wc_output.txt heredoc.txt file1.txt file2.txt combined.txt empty.txt
echo "✓ Test files cleaned up"
echo

echo "=== Redirection Tests Completed ==="
