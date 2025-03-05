
def count_equal_last_columns(file_path):
    # Initialize a counter for equal last columns
    equal_count = 0
    total_counter = 0
    
    # Open the file and read line by line
    with open(file_path, 'r') as file:
        for line in file:
            total_counter += 1
            # Split the line into columns
            columns = line.strip().split()
            # Check if the last two columns are equal
            if columns[-1] == columns[-2]:
                equal_count += 1
    
    # Return the count of equal last columns
    return equal_count, total_counter

def data_freq():
    data = dict()
    with open("predictions.dat", 'r') as file:
        for line in file:
            # Split the line into columns
            columns = line.strip().split()
            # Check if the last two columns are equal
            if data.get(columns[-2]) == None:
                data[columns[-2]] = 0
            else:
                data[columns[-2]] = data[columns[-2]]+1
    print(dict(sorted(data.items(), key=lambda item: item[1])))

def data_freq_pred():
    data = dict()
    with open("predictions.dat", 'r') as file:
        for line in file:
            # Split the line into columns
            columns = line.strip().split()
            # Check if the last two columns are equal
            if data.get(columns[-1]) == None:
                data[columns[-1]] = 0
            else:
                data[columns[-1]] = data[columns[-1]]+1

    print(dict(sorted(data.items(), key=lambda item: item[1])))

# Example usage (the file path needs to be replaced with the actual file path):
# equal_last_columns_count = count_equal_last_columns('data.txt')
# print(f"The last two columns are equal {equal_last_columns_count} times.")
correct, total = count_equal_last_columns("prediction.dat")
print(correct * 1.0/total)
data_freq()
data_freq_pred()
