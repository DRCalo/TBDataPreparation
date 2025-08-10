import csv



def main():

    # Read the source CSV file and store relevant columns in a dictionary
    source_data = {}
    with open('pedestals.csv', 'r') as source_file:
        reader = csv.DictReader(source_file)
        for row in reader:
            source_data[row['Tower']] = {'S_ped': row['S_ped'], 'C_ped': row['C_ped']}

    # Open the existing CSV file and prepare to write the merged content
    with open('peaks.csv', 'r') as existing_file, open('merged.csv', 'w') as merged_file:
        reader = csv.DictReader(existing_file)
        fieldnames = reader.fieldnames + ['S_ped', 'C_ped']  # Combine columns
        
        writer = csv.DictWriter(merged_file, fieldnames=fieldnames)
        writer.writeheader()
        
        for row in reader:
            # Update the row with the data from the source CSV file
            if row['Tower'] in source_data:
                row.update(source_data[row['Tower']])
            writer.writerow(row)

if __name__=="__main__":
    main()
