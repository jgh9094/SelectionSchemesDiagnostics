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

######################## IMPORTS ########################
import datetime
import argparse
import os
import pandas as pd

######################## VARIABLES EVERYONE NEEDS TO KNOW ########################
REPLICATION_OFFSET=0
DATA_DIR='/mnt/scratch/herna383/SelectionData/'
POP_FILE = "/pop_stats.csv"
REPLICATES = 100
COL = 1
NOW = datetime.datetime.now()


######################## LEXICASE ########################
LEX_POP_SIZE = ['10', '100', '500', '700', '1000']
LEX_OFFSET = 0
LEX_DIR_1 = "SEL_LEXICASE__DIA_Exploitation__POP_"
LEX_DIR_2 = "__TRT_100__SEED_"

def lex(d_dir, w_dir, snap):
    print('-----------------------------'*4)
    print('Processing Lexicase Runs for Average Error-' + str(NOW))

    # Go though all treatment configurations
    for i in range(len(LEX_POP_SIZE)):

        # Set up the directory
        dir = d_dir + LEX_DIR_1 + LEX_POP_SIZE[i] + LEX_DIR_2
        # Store all the frames and headers
        frames = []
        header = []

        # Go through each replicate
        for r in range(1,REPLICATES+1):
            # Calculate Seed
            seed = (r + (i * 100)) + LEX_OFFSET + REPLICATION_OFFSET

            # Check if data directory exists
            if(os.path.isdir(dir + str(seed))):
                # Create data frame
                data = pd.read_csv(dir + str(seed) + POP_FILE, index_col=False)

                # Grab every nth row
                data = data.iloc[::snap, COL]
                frames.append(data)

                # Add replicate number to the header
                header.append('r'+ str(r))

        result = pd.concat(frames, axis=1, join='inner',ignore_index=True)
        result.insert(result.shape[1], 'pop', [LEX_POP_SIZE[i]]* result.shape[0], True)
        header.append('pop')
        result.to_csv("lex_pop_avg_err_" + str(LEX_POP_SIZE[i]) + ".csv", sep=',', header=header, index=True, index_label="Generation")

    # We have finished!
    print('-----------------------------'*4)
    print()

######################## TOURNAMENT ########################
TRN_SIZE = ['7', '100', '500', '700', '1000']
TRN_OFFSET = 500
TRN_DIR_1 = "SEL_TOURNAMENT__DIA_Exploitation__POP___TRT_100__TOURN_"
TRN_DIR_2 = "__SEED_"

def tour(d_dir, w_dir, snap):
    print('-----------------------------'*4)
    print('Processing Tournament Runs for Average Error-' + str(NOW))

    # Go though all treatment configurations
    for i in range(len(TRN_SIZE)):

        # Set up the directory
        dir = d_dir + TRN_DIR_1 + TRN_SIZE[i] + TRN_DIR_2
        # Store all the frames and headers
        frames = []
        header = []

        # Go through each replicate
        for r in range(1,REPLICATES+1):
            # Calculate Seed
            seed = (r + (i * 100)) + TRN_OFFSET + REPLICATION_OFFSET

            # Check if data directory exists
            if(os.path.isdir(dir + str(seed))):
                # Create data frame
                data = pd.read_csv(dir + str(seed) + POP_FILE, index_col=False)

                # Grab every nth row
                data = data.iloc[::snap, COL]
                frames.append(data)

                # Add replicate number to the header
                header.append('r'+ str(r))

        result = pd.concat(frames, axis=1, join='inner',ignore_index=True)
        result.insert(result.shape[1], 'pop', [TRN_SIZE[i]]* result.shape[0], True)
        header.append('pop')
        result.to_csv("trn_pop_avg_err_" + str(TRN_SIZE[i]) + ".csv", sep=',', header=header, index=True, index_label="Generation")

    # We have finished!
    print('-----------------------------'*4)
    print()

######################## DOWN SAMPLED ########################
DSL_PROP = ['.05', '.10', '.25', '.50', '1.0']
DSL_OFFSET = 1500
DSL_DIR_1 = "SEL_DOWN__DIA_Exploitation__POP_1000__TRT_100__PROP_"
DSL_DIR_2 = "__SEED_"

def dsl(d_dir, w_dir, snap):
    print('-----------------------------'*4)
    print('Processing Down Sampled Runs-' + str(NOW))


    for i in range(len(DSL_PROP)):

        # Set up the directory
        dir = d_dir + DSL_DIR_1 + DSL_PROP[i] + DSL_DIR_2

        # Store all the frames and headers
        frames = []
        header = []

        for r in range(1,REPLICATES+1):
            # Calculate Seed
            seed = (r + (i * 100)) + DSL_OFFSET + REPLICATION_OFFSET
            print(dir + str(seed) + POP_FILE)
            # Check to see if directory exists
            if(os.path.isdir(dir + str(seed))):

                # Create data frame
                data = pd.read_csv(dir + str(seed) + POP_FILE, index_col=False)

                # Grab every nth row
                data = data.iloc[::snap, COL]

                frames.append(data)

                # Add replicate number to the header
                header.append('r'+ str(r))

        result = pd.concat(frames, axis=1, join='inner',ignore_index=True)
        result.insert(result.shape[1], 'pop', [DSL_PROP[i]]* result.shape[0], True)
        header.append('pop')
        result.to_csv("dsl_pop_avg_err_" + str(DSL_PROP[i]) + ".csv", sep=',', header=header, index=True, index_label="Generation")

    # We have finished!
    print('-----------------------------'*4)
    print()

######################## DOWN SAMPLED ########################
COH_PROP = ['.05', '.10', '.25', '.50', '1.0']
COH_OFFSET = 1000
COH_DIR_1 = "SEL_COHORT__DIA_Exploitation__POP_1000__TRT_100__PROP_"
COH_DIR_2 = "__SEED_"

def coh(d_dir, w_dir, snap):
    print('-----------------------------'*4)
    print('Processing Down Sampled Runs-' + str(NOW))


    for i in range(len(COH_PROP)):

        # Set up the directory
        dir = d_dir + COH_DIR_1 + COH_PROP[i] + COH_DIR_2

        # Store all the frames and headers
        frames = []
        header = []

        for r in range(1,REPLICATES+1):
            # Calculate Seed
            seed = (r + (i * 100)) + COH_OFFSET + REPLICATION_OFFSET
            print(dir + str(seed) + POP_FILE)
            # Check to see if directory exists
            if(os.path.isdir(dir + str(seed))):

                # Create data frame
                data = pd.read_csv(dir + str(seed) + POP_FILE, index_col=False)

                # Grab every nth row
                data = data.iloc[::snap, COL]

                frames.append(data)

                # Add replicate number to the header
                header.append('r'+ str(r))

        result = pd.concat(frames, axis=1, join='inner',ignore_index=True)
        result.insert(result.shape[1], 'pop', [COH_PROP[i]]* result.shape[0], True)
        header.append('pop')
        result.to_csv("coh_pop_avg_err_" + str(COH_PROP[i]) + ".csv", sep=',', header=header, index=True, index_label="Generation")

    # We have finished!
    print('-----------------------------'*4)
    print()

def main():
    # Generate the arguments
    parser = argparse.ArgumentParser(description="Data aggregation script.")
    parser.add_argument("data_directory", type=str, help="Target experiment directory.")
    parser.add_argument("dump_directory", type=str, help="Target dump directory")
    parser.add_argument("selection", type=int, help="Snapshot at each generation")
    parser.add_argument("snapshot", type=int, help="Snapshot at each generation")

    # Get the arguments
    args = parser.parse_args()
    data_directory = args.data_directory.strip()
    write_directory = args.dump_directory.strip()
    sel = args.selection
    snapshot = args.snapshot

    if(sel == 0):
        lex(data_directory, write_directory, snapshot)

    elif(sel == 1):
        tour(data_directory, write_directory, snapshot)

    elif(sel == 2):
        coh(data_directory, write_directory, snapshot)

    elif(sel == 3):
        dsl(data_directory, write_directory, snapshot)

    # elif(sel == 2):
    #     dsl(data_directory, write_directory, snapshot)


if __name__ == "__main__":
    main()