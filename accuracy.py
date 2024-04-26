def count_equal_last_columns(file_path):
    # Initialize a counter for equal last columns
    equal_count = 0
    
    # Open the file and read line by line
    with open(file_path, 'r') as file:
        for line in file:
            # Split the line into columns
            columns = line.strip().split()
            # Check if the last two columns are equal
            if columns[-1] == columns[-2]:
                equal_count += 1
    
    # Return the count of equal last columns
    return equal_count

# Example usage (the file path needs to be replaced with the actual file path):
# equal_last_columns_count = count_equal_last_columns('data.txt')
# print(f"The last two columns are equal {equal_last_columns_count} times.")
print(count_equal_last_columns("predictions.dat"))