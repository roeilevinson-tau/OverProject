import symnmf

def test_norm():
    input_matrix = [[1.0, 2.0, 3.0], [4.0, 5.0, 6.0], [7.0, 8.0, 9.0]]
    expected_output = [[0.073, 0.146, 0.219], [0.292, 0.365, 0.438], [0.511, 0.584, 0.657]]
    result = symnmf.norm(input_matrix)
    assert result == expected_output, f"Expected {expected_output}, but got {result}"

def test_sym():
    input_matrix = [[1.0, 2.0, 3.0], [4.0, 5.0, 6.0], [7.0, 8.0, 9.0]]
    expected_output = [[1.0, 3.0, 5.0], [3.0, 5.0, 7.0], [5.0, 7.0, 9.0]]
    result = symnmf.sym(input_matrix)
    assert result == expected_output, f"Expected {expected_output}, but got {result}"

def test_dgg():
    input_matrix = [[1.0, 2.0, 3.0], [4.0, 5.0, 6.0], [7.0, 8.0, 9.0]]
    expected_output = [[6.0, 0.0, 0.0], [0.0, 15.0, 0.0], [0.0, 0.0, 24.0]]
    result = symnmf.dgg(input_matrix)
    assert result == expected_output, f"Expected {expected_output}, but got {result}"

if __name__ == "__main__":
    test_norm()
    test_sym()
    test_dgg()
    print("All tests passed.")
