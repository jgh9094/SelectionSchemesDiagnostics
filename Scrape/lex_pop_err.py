# Will list all of the incomplete id's that need to finish running
# SEED_ORIGINAL & TREATMENTS  will need to be adjusted based on the problems, treatments, and seeds that the project requires.
# Will also need to handle RANGE if different from the expected results!
#
# Input 1: file directory where all the folders are located
# Input 2: Directory where the new csv files will be placed
# Input 3: Print Interval!
#
# Output : create a csv for each population size
#
# python3

POP_SIZE = [10, 100, 500, 700, 1000]
OFFSET = 0
POP_FILE = "/pop_stats.csv"
COL = 1
REPLICATES = 100
DIRECTORY = "SEL_LEXICASE__DIA_Exploitation__POP_"

import argparse
import pandas as pd

def main():
    # Generate the arguments
    parser = argparse.ArgumentParser(description="Data aggregation script.")
    parser.add_argument("data_directory", type=str, help="Target experiment directory.")
    parser.add_argument("dump_directory", type=str, help="Target dump directory")
    parser.add_argument("snapshot", type=int, help="Snapshot at each generation")

    # Get the arguments
    args = parser.parse_args()
    data_directory = args.data_directory
    write_directory = args.dump_directory
    write_directory = write_directory.strip()
    snapshot = args.snapshot

    count = 0

    # Iterate through each pop size
    for i in range(len(POP_SIZE)):
        dir = data_directory + DIRECTORY + str(POP_SIZE[i]) + "__TRT_100__SEED_"
        # Iterate through all the seed replicates
        for s in range(1,REPLICATES+1):
            dir = dir + str(s + (i * 100)) + POP_FILE
            data = pd.read_csv(dir)
            print(data.head(1))


            count = count + 1
            dir = data_directory + DIRECTORY + str(POP_SIZE[i]) + "__TRT_100__SEED_"


    print("count=",count)

if __name__ == "__main__":
    main()