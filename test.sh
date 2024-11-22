#!/bin/bash

# Build the C program
make

# Build the Python extension
python3 setup.py build_ext --inplace

# Function to compare outputs
compare_outputs() {
    local test_name=$1
    local c_output="c_output.txt"
    local py_output="py_output.txt"
    
    echo "Running test: $test_name"
    
    # Run C and Python commands and save outputs
    $2 > "$c_output" 2>/dev/null
    $3 > "$py_output" 2>/dev/null
    
    # Compare outputs
    if diff -q "$c_output" "$py_output" >/dev/null; then
        echo "✓ $test_name: Outputs match"
    else
        echo "✗ $test_name: Outputs differ"
        echo "Differences:"
        diff "$c_output" "$py_output"
    fi
    echo "----------------------------------------"
}

# Test case 1
echo "Testing input_1.txt (k=5)..."
compare_outputs "similarity_matrix_1" "./symnmf sym tests/input_1.txt" "python3 symnmf.py 5 sym tests/input_1.txt"
compare_outputs "diagonal_degree_matrix_1" "./symnmf ddg tests/input_1.txt" "python3 symnmf.py 5 ddg tests/input_1.txt"
compare_outputs "normalized_matrix_1" "./symnmf norm tests/input_1.txt" "python3 symnmf.py 5 norm tests/input_1.txt"

# Test case 2
echo "Testing input_2.txt (k=4)..."
compare_outputs "similarity_matrix_2" "./symnmf sym tests/input_2.txt" "python3 symnmf.py 4 sym tests/input_2.txt"
compare_outputs "diagonal_degree_matrix_2" "./symnmf ddg tests/input_2.txt" "python3 symnmf.py 4 ddg tests/input_2.txt"
compare_outputs "normalized_matrix_2" "./symnmf norm tests/input_2.txt" "python3 symnmf.py 4 norm tests/input_2.txt"

# Test case 3
echo "Testing input_3.txt (k=7)..."
compare_outputs "similarity_matrix_3" "./symnmf sym tests/input_3.txt" "python3 symnmf.py 7 sym tests/input_3.txt"
compare_outputs "diagonal_degree_matrix_3" "./symnmf ddg tests/input_3.txt" "python3 symnmf.py 7 ddg tests/input_3.txt"
compare_outputs "normalized_matrix_3" "./symnmf norm tests/input_3.txt" "python3 symnmf.py 7 norm tests/input_3.txt"

# Cleanup temporary files
rm -f c_output.txt py_output.txt 