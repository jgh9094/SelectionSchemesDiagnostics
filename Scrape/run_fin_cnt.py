# Will go thourgh each directory and see if the folder exists and count the
# number of finsihed runs.
#
#
# Output : print out the seeds that are not finished running for a treatment replicate
#
# python3

# Imports
import datetime
import argparse
import os

# VARIABLES EVERYONE NEEDS TO KNOW
GENRATIONS=50000
REPLICATION_OFFSET=0
DATA_DIR='/mnt/scratch/herna383/SelectionData/'
POP_FILE = "/pop_stats.csv"
REPLICATES = 100
NOW = datetime.datetime.now()


############ LEXICASE STUFF ############
LEX_POP_SIZE = [10, 100, 500, 700, 1000]
LEX_OFFSET = 0
LEX_DIR_1 = "SEL_LEXICASE__DIA_Exploitation__POP_"
LEX_DIR_2 = "__TRT_100__SEED_"

def lex(d_dir):
  print('-----------------------------')
  print('Processing Lexicase Runs-' + str(NOW))

  for i in range(len(LEX_POP_SIZE)):
    for r in range(1,REPLICATES+1):
      # Create the directory
      seed = (r + (i * 100)) + LEX_OFFSET
      dir = d_dir + LEX_DIR_1 + str(LEX_POP_SIZE[i]) + LEX_DIR_2 + str(seed)

      # Check to see if directory exists
      if(os.path.isdir(dir)):
        print(dir + '===FOUND')

      else:
        print(dir + '===NOTEXISTING')



def main():
  # Generate the arguments
  parser = argparse.ArgumentParser(description="Data aggregation script.")
  parser.add_argument("data_directory", type=str, help="Target experiment directory.")

  # Get the arguments
  args = parser.parse_args()
  data_directory = args.data_directory
  print('Starting run_fin_cnt.py')
  lex(data_directory)


if __name__ == "__main__":
    main()