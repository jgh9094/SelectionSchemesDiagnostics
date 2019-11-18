/// This is the world for DiaOrgs

/* TODO SECTION

[X]: Finished coding >:D
[P]: Process of coding >:|
[?]: Need to figure out still :?
[]: Nothing done yet :)

SELECTIONS:
[X]: Tournament
[X]: Lexicase
[X]: Drift
[P]: Cohort Lexicase
[]: Down Sampled Lexicase
[]: EcoEA
[]: Roulette

DIAGNOSTICS:
[X]: Exploitation
[X]: Structured Exploitation
[]: Contradictory K Traits Ecologoy
[]: Weak Ecology
[]: Specialist
[]: Good & Bad Hints
[]: Bias
[]: Deceptive
[]: Overfitting With Noise
[?]: Overfitting With Smoothness
[]: Exploitation

DATA Analysis
[X]: Triat Average Error
[X]: Trait Min Error
[X]: Trait Solution Count
[X]: Population Average Error
*/

///< Defnining name
#ifndef DIA_WORLD_H
#define DIA_WORLD_H

///< Standard headers
#include <functional>
#include <utility>
#include <cmath>
#include <fstream>
#include <string>

///< Empirical headers
#include "Evolve/World.h"
#include "base/vector.h"

///< Experiment headers
#include "config.h"
#include "DiagnosticOrg.h"

///< Custom naming
using ids_t = emp::vector<size_t>;
using fit_agg_t = std::function<double(DiaOrg &)>;
using fit_lex_t = std::function<double(DiaOrg &, size_t)>;
using eva_fun = std::function<void(DiaOrg &)> ;
using sel_fun = std::function<ids_t()>;
using tar_t = emp::vector<double>;
using cls_t = emp::vector<emp::vector<size_t>>;

class DiaWorld : public emp::World<DiaOrg> {

  private:

    /* Experiment Set Up */
    DiaWorldConfig & config;          ///< Experiments configurations
    fit_agg_t fitness_agg;            ///< Variable to return aggregate errors
    fit_lex_t fitness_lex;            ///< Variable to fitness for a specific case
    sel_fun select;                   ///< Experiment selection fucntion
    eva_fun evaluate;                 ///< Experiment evaluation function

    /* Population, Org, and Traits */
    ids_t pop_ids;                    ///< Population IDs for randomly picking from the world
    ids_t trait_ids;                  ///< Vector holding ids for each trait we are evaluating
    tar_t target;                     ///< Targets that organisms are trying to reach
    cls_t pop_coh;                    ///< Hold the organism cohorts
    cls_t trt_coh;                    ///< Hold the trait cohorts

    /* Data Tracking */
    std::fstream trt_sol_cnt;         ///< Track number of solutions for a trait per update
    std::fstream trt_avg_err;         ///< Track the average error for a trait per update
    std::fstream trt_min_err;         ///< Track the minimum error for a trait  per update
    std::fstream pop_avg_err;         ///< Track the average error for the population each generation

  public:
    DiaWorld(DiaWorldConfig & _config) : config(_config) {      ///< Constructor
      // Initialize the vector to poplulation pop_ids
      for(size_t i = 0; i < config.POP_SIZE(); i++) {pop_ids.push_back(i);}
      // Initialze the  vector t0 traits trait_ids
      for(size_t i = 0; i < config.K_TRAITS(); i++) {trait_ids.push_back(i);}
      // Set requested selection and fitness functions
      InitialSetup();
      // Initialize pointer
      random_ptr = emp::NewPtr<emp::Random>(config.SEED());

      // If not multiobjective, then we can set all traits to the same value
      if(!config.MULTIOBJECTIVE()) {
        std::cerr << "MULTIOBJECTIVE: False" << std::endl;
        target.resize(config.K_TRAITS(), config.TARGET());
      }
      else {

      }
    }

    ~DiaWorld() {
      trt_sol_cnt.close();
      trt_avg_err.close();
      trt_min_err.close();
    }

    /* Functions for initial experiment set up */

    void InitialSetup();            ///< Do all the initial set up
    void SetOnUpdate();             ///< Set up world configurations
    void SetMutations();            ///< Set up the mutations parameters
    void SetSelectionFun();         ///< Set up the selection function
    void SetOnOffspringReady();     ///< Set up the OnOffspringReady function
    void SetEvaluationFun();        ///< Set up the Evaluation function
    void InitializeWorld();         ///< Set initial population of orgs


    /* Functions for Tournament Selection set up */

    void TournamentFitnessFun();        ///< Set fitness function for Tournament
    void TournamentSelection();         ///< Set Tournament Selection Algorithm
    void TournamentExploit();           ///< Set fitness function as Exploitation
    void TournamentStructExploit();     ///< Set fitness function as Structured Exploitation


    /* Functions for Lexicase Selection set up */

    void LexicaseFitnessFun();                                                ///< Set fitness function for Lexicase
    void LexicaseSelection();                                                 ///< Set Lexicase Selection Algorithm
    size_t LexicaseWinner(ids_t & round_pop, ids_t const & test_cases);       ///< Will select a parent winner
    void LexicaseExploit();                                                   ///< Set fitness function as Exploitation
    void LexicaseStructExploit();                                             ///< Set fitness function as Structured Exploitation


    /* Function for Drift Selction set up */

    void DriftFitnessFun();
    void DriftSelection();


    /* Function for Drift Selction set up */

    void CohortLexicaseFitnessFun();          ///< Set fitness function for Lexicase
    void CohortLexicaseSelection();           ///< Set Lexicase Selection Algorithm
    void CohortLexicaseInitialize();          ///< Create the cohorts for both population and traits


    /* Functions for Evaluation set up */

    void EvalExploit();               ///< Evaluate organisms with exploitation metric
    void EvalStructExploit();         ///< Evalate organisms with structured exploitation metric


    /* Functions ran during experiment */

    void Evaluate();                ///< Evaluate all orgs on individual test cases!
    ids_t Selection();              ///< Call when its time to select parents
    size_t Mutate();                ///< Call when we need to mutate an organism
    void Births(ids_t parents);                  ///< Call when its time to produce offspring


    /* Functions for gathering data */

    void SetCSVHeaders();                        ///< Set up all headers of csv files!
    void GatherData(size_t up);                  ///< Will call all other functions to gather data
    void CountTraitSolution(size_t up);          ///< Given some threshold, how many solutions do we have?
    void AverageTraitError(size_t up);           ///< Average error on a trait, per population, per update
    void MinimumTraitError(size_t up);           ///< Get best error per trait
    void AveragePopError(size_t up);             ///< Average error for a population per update
};

/* Functions for initial experiment set up */

void DiaWorld::InitialSetup() {     ///< Do all the initial set up
  Reset();
  SetPopStruct_Mixed(true);
  SetMutations();
  SetSelectionFun();
  SetOnOffspringReady();
  SetEvaluationFun();
  InitializeWorld();
  SetOnUpdate();
  SetCSVHeaders();
}

void DiaWorld::SetOnUpdate() {      ///< Set up world configurations
  OnUpdate([this](size_t) {
    // Evaluate all organisms
    Evaluate();
    // Gather relavant data!
    GatherData(GetUpdate());
    // Select parents for next gen
    auto parents = Selection();
    // Give birth to the next gen & mutate
    Births(parents);
  });
}

void DiaWorld::SetMutations() {     ///< Set up the mutations parameters
  // Set up the worlds mutation function!
  SetMutFun([this](DiaOrg & org, emp::Random & random) {
    // Get org genome!
    auto & genome = org.GetGenome();
    size_t cnt = 0;
    // Check that genome is correct length
    emp_assert(genome.size() == config.K_TRAITS(), genome.size());

    // Loop through and apply mutations if applicable
    for(size_t i = 0; i < genome.size(); i++) {
      // Can we do a mutation?
      if(random_ptr->P(config.MUTATE_VAL())) {
        // Apply mutation from normal distribution
        double evo = random_ptr->GetRandNormal(config.MEAN(), config.STD());
        genome[i] += evo;
        cnt += 1;
      }
    }

    return cnt;
  });
}

void DiaWorld::SetSelectionFun() {  ///< Set up the selection function
  switch (config.SELECTION())
  {
  case 0:  // Tournament
    std::cerr << "SELECTION: "; TournamentSelection();
    std::cerr << "DIAGNOSTIC: "; TournamentFitnessFun();
    break;

  case 1: // Lexicase
    std::cerr << "SELECTION: "; LexicaseSelection();
    std::cerr << "DIAGNOSTIC: "; LexicaseFitnessFun();
    break;

  case 2: // Drift
    std::cerr << "SELECTION: "; DriftFitnessFun();
    std::cerr << "DIAGNOSTIC: "; DriftSelection();
    break;

  default:
    std::cerr << "SELECTED INVALID SELECTION SCHEME" << std::endl;
    exit(-1);
    break;
  }
}

void DiaWorld::SetEvaluationFun() {        ///< Set up the Evaluation function
  // What function are using to evaluate?
  std::cerr << "Evaluation Function: ";
  switch (config.DIAGNOSTIC())
  {
  case 0: // Exploitation
    EvalExploit();
    break;

  case 1: // Structured Exploitation
    EvalStructExploit();
    break;

  default:
    std::cerr << "NO EVALUATION MODE EXISTS" << std::endl;
    exit(-1);
    break;
  }
}

void DiaWorld::SetOnOffspringReady() {  ///< Set up the OnOffspringReady function
  // Create lambda function
  OnOffspringReady([this](DiaOrg & org, size_t parent_pos) {
    // Mutate organism if possible
    if(config.MUTATE()) {
      DoMutationsOrg(org);
      org.Reset(config.K_TRAITS());
    }
  });
}

void DiaWorld::InitializeWorld() {  ///< Set initial population of orgs
  // Fill the workd with requested population size!
  DiaOrg org(config.K_TRAITS());
  Inject(org.GetGenome(), config.POP_SIZE());
}


/* Functions for Tournament Selection set up */

void DiaWorld::TournamentFitnessFun() {
  // Set up the cases to set the fitness fuction
  switch (config.DIAGNOSTIC()) {
    case 0:  // Exploitation
      TournamentExploit();
      break;

    case 1:  // Structured Exploitation
      TournamentStructExploit();
      break;

    case 2:  // Ecology Diagnostic - Contradictory K Values
      /* code */
      break;

    case 3:  // Ecology Diagnostic
      /* code */
      break;

    case 4:  // Specialist
      /* code */
      break;

    case 5:  // Hints
      /* code */
      break;

    case 6:  // Bias
      /* code */
      break;

    case 7:  // Deceptive
      /* code */
      break;

    case 8:  // Overfitting - Noise
      /* code */
      break;

    case 9:  // Exploration
      /* code */
      break;

    default:  // Error if we reach here
      std::cerr << "TRYING TO SET A DIAGNOSTIC THAT DOESN'T EXIST" << std::endl;
      exit(-1);
      break;
  }
}

void DiaWorld::TournamentSelection() {
  std::cerr << "Tournament Selection" << std::endl;
  emp_assert(config.TOUR_SIZE() <= config.POP_SIZE(), config.TOUR_SIZE());
  // Setting up tournament selection algorithm
  select = [this] () {
    // Holds all ids of parents selected for reproduction
    ids_t parents;

    // Keep going till we have enough parents!
    while(parents.size() != config.POP_SIZE()) {
      // Get ids for a tournament to begin
      auto tour = emp::Choose(*random_ptr, config.POP_SIZE(), config.TOUR_SIZE());
      std::map<double, ids_t> scores;

      // Get all the fitnesses
      for(auto i : tour) {
        // Get org fitness
        double score = fitness_agg(GetOrg(i));

        // Check if we have seen this score
        auto it = scores.find(score);
        // Not found
        if(it == scores.end()) {
          ids_t fresh;
          fresh.push_back(i);
          scores[score] = fresh;
        }
        else {
          scores[score].push_back(i);
        }
      }
      // Randomly select a parent if there are ties, else get the only parent
      size_t winner = emp::Choose(*random_ptr, scores.begin()->second.size(), 1)[0];
      parents.push_back(scores.begin()->second[winner]);
    }
    return parents;
  };
}

void DiaWorld::TournamentExploit(){
  std::cerr << "Standard Exploitation" << std::endl;
  fitness_agg = [this] (DiaOrg & org) {
    // Return the sum of errors!
    return org.GetTotalScore();
  };
}

void DiaWorld::TournamentStructExploit() {    ///< Set fitness function as Structured Exploitation
  std::cerr << "Structured Exploitation" << std::endl;
  fitness_agg = [this] (DiaOrg & org) {
    // Return the sum of errors!
    return org.GetTotalScore();
  };
}


/* Functions for Lexicase Selection set up */

void DiaWorld::LexicaseFitnessFun() {        ///< Set fitness function for Lexicase
  // Set up the cases to set the fitness fuction
  switch (config.DIAGNOSTIC()) {
    case 0:  // Exploitation
      LexicaseExploit();
      break;

    case 1:  // Structured Exploitation
      LexicaseStructExploit();
      break;

    case 2:  // Ecology Diagnostic - Contradictory K Values
      /* code */
      break;

    case 3:  // Ecology Diagnostic
      /* code */
      break;

    case 4:  // Specialist
      /* code */
      break;

    case 5:  // Hints
      /* code */
      break;

    case 6:  // Bias
      /* code */
      break;

    case 7:  // Deceptive
      /* code */
      break;

    case 8:  // Overfitting - Noise
      /* code */
      break;

    case 9:  // Exploration
      /* code */
      break;

    default:  // Error if we reach here
      std::cerr << "TRYING TO SET A DIAGNOSTIC THAT DOESN'T EXIST" << std::endl;
      exit(-1);
      break;
  }
}

void DiaWorld::LexicaseSelection() {         ///< Set Lexicase Selection Algorithm
  // What are we doing?
  std::cerr << "Lexicase Selection" << std::endl;

  // Check if we even have test cases lol
  emp_assert(trait_ids.size() > 0, trait_ids.size());
  emp_assert(pop_ids.size() == config.POP_SIZE(), pop_ids.size());

  // Set the selction lambda
  select = [this] () {
    // Will hold all the parent ids for repreduction
    ids_t parents;

    // Keep looping through until we have enough parents
    while(parents.size() != config.POP_SIZE()) {
      // Shuffle test cases
      emp::Shuffle(*random_ptr, trait_ids);
      // this list of parents will be used by lexicase to single out a parent
      ids_t round_winners = pop_ids;
      // Get the winning parent
      size_t winner = LexicaseWinner(round_winners, trait_ids);
      // Add parent to the next gen
      parents.push_back(winner);
    }

    return parents;
  };
}

///< This function will take in an initial population and trim it down
size_t DiaWorld::LexicaseWinner(ids_t & round_pop, ids_t const & test_cases) {
  // Will hold all the scores with associated ids
  std::map<double, ids_t> scores;
  // Position trackers for test cases
  size_t tc = 0;

  // Check if rounds_winners is the correct size at the beggining!
  emp_assert(round_pop.size() == config.POP_SIZE(), round_pop.size());

  // Loop through all the test cases until we have a single winner or
  // run out of test cases to use! Will randomly select winner if more than one
  // at the end of the round.
  while(tc < test_cases.size() && round_pop.size() != 1) {
    // Clear scores for this round!
    scores.clear();

    // std::cerr << tc << std::endl;

    // Loop through all the winners per round
    for(size_t oid : round_pop) {
      //Get trait we are looking at and org score on trait
      size_t trait = test_cases[tc];
      double score = fitness_lex(GetOrg(oid), trait);

      // Have we seen this score before?
      auto it = scores.find(score);
      // Not seen before
      if(it == scores.end()) {
        ids_t fresh = {oid};
        scores[score] = fresh;
      }
      else {
        scores[score].push_back(oid);
      }
    }

    // Set the next round winners and update all vars
    round_pop = scores.begin()->second;
    tc++;
  }

  // At this point we should have our finalists
  // We will return one winner no matter the size of the vector of ids!
  size_t winner = emp::Choose(*random_ptr, scores.begin()->second.size(), 1)[0];
  return scores.begin()->second[winner];
}

void DiaWorld::LexicaseExploit() {           ///< Set fitness function as Exploitation
  std::cerr << "Exploitation" << std::endl;
  fitness_lex = [this] (DiaOrg & org, size_t i) {
    return org.GetScore(i);
  };
}

void DiaWorld::LexicaseStructExploit() {     ///< Set fitness function as Structured Exploitation
  std::cerr << "Exploitation" << std::endl;
  fitness_lex = [this] (DiaOrg & org, size_t i) {
    return org.GetScore(i);
  };
}


/* Function for Drift Selction set up */

void DiaWorld::DriftFitnessFun() {
  /* Fitness does not matter for this case
     but function if user would like the user
     would like to track additional things! */

     std::cerr << "Drift Requires No Fitness!" << std::endl;
}

void DiaWorld::DriftSelection() {
  // What are we doing?
  std::cerr << "Drift Selection" << std::endl;

  // Check some stuff before hand
  emp_assert(pop_ids.size() == config.POP_SIZE(), pop_ids.size());

  select = [this] () {
    // Will hold all the parent ids for repreduction
    ids_t parents;

    // Keep looping through until we have enough parents
    while(parents.size() != config.POP_SIZE()) {
      size_t pick = emp::Choose(*random_ptr, config.POP_SIZE(), 1)[0];
      parents.push_back(pop_ids[pick]);
    }

    return parents;
  };

}


/* Functions ran during experiment */

void DiaWorld::Evaluate() {          ///< Evaluate all orgs on individual test cases!
  //Make sure pop is the correct size
  emp_assert(pop.size() == config.POP_SIZE(), pop.size());
  emp_assert(target.size() == config.K_TRAITS(), target.size());

  // Loop through world and score orgs
  for(size_t pos = 0; pos < pop.size(); pos++) {
    // Get org and calculate score
    auto & org = *pop[pos];
    org.Reset(config.K_TRAITS());

    // Evaluate orgs!
    evaluate(org);

    // Sum up all the error for the orgs
    org.TotalSumScores();
  }
}

ids_t DiaWorld::Selection() {       ///< Call when its time to select parents
  return select();
}

void DiaWorld::Births(ids_t parents) {            ///< Call when its time to produce offsprings from parents
  // Go through the parent ids and give birth!
  for(auto & id : parents) {
    DoBirth(GetGenomeAt(id), id);
  }
}


/* Functions for Evaluation set up */

void DiaWorld::EvalExploit() {      ///< Evaluate organisms with exploitation metrics
  std::cerr << "Exploitation" << std::endl;
  // Set the evaluation function
  evaluate = [this] (DiaOrg & org) {
    for(size_t i = 0; i < target.size(); i++) {
      // Calculate individual values
      org.ExploitError(i, target[i]);
    }
  };
}

void DiaWorld::EvalStructExploit() {         ///< Evalate organisms with structured exploitation metric
  std::cerr << "Structured Exploitation" << std::endl;
  // Set up structured exploitation function
  evaluate = [this] (DiaOrg & org) {
    org.StructExploitError(target);
  };
}


/* Functions for gathering data */

void DiaWorld::SetCSVHeaders() {                    ///< Set up all headers of csv files!
  std::cerr << "SETTING CSV HEADER" << std::endl;

  // Initialize data trackers
  trt_avg_err.open(config.OUTPUT_DIR() + "trt_avg_err.csv", std::ios::out | std::ios::app);
  trt_min_err.open(config.OUTPUT_DIR() + "trt_min_err.csv", std::ios::out | std::ios::app);
  trt_sol_cnt.open(config.OUTPUT_DIR() + "trt_sol_cnt.csv", std::ios::out | std::ios::app);
  pop_avg_err.open(config.OUTPUT_DIR() + "pop_avg_err.csv", std::ios::out | std::ios::app);

  // Create header
  std::string head = "Update,";
  for(size_t i = 0; i < config.K_TRAITS(); i++) {
    std::string cur = (i < config.K_TRAITS()-1) ? "val-" + std::to_string(i) + "," : "val-" + std::to_string(i);
    head += cur;
  }

  trt_avg_err << head << std::endl;
  trt_min_err << head << std::endl;
  trt_sol_cnt << head << std::endl;
  pop_avg_err << "Gen,Error" << std::endl;
}

void DiaWorld::GatherData(size_t up) {               ///< Will call all other functions to gather data
  AverageTraitError(up);
  MinimumTraitError(up);
  CountTraitSolution(up);
  AveragePopError(up);
}

void DiaWorld::CountTraitSolution(size_t up) {            ///< Given some threshold, how many solutions do we have?
  // Hold solution counts
  emp::vector<size_t> record;
  record.resize(config.K_TRAITS(), 0);
  // Iterate through the internal values
  for(size_t k = 0; k < config.K_TRAITS(); k++) {
    // Iterate through the pop per internal val
    for(size_t i = 0; i < pop.size(); i++) {
      // Get org and its score on the ith internal k val
      DiaOrg & org = *pop[i];
      double score = org.GetScore(k);

      // Check if it meets the threshold
      if(score <= config.SOLUTION_THRESH()) {
        record[k]++;
      }
    }
  }

  // Write all data to the csv
  std::string update = std::to_string(up) + ",";
  for(size_t i = 0; i < config.K_TRAITS(); i++) {
    std::string cur = (i < config.K_TRAITS()-1) ? std::to_string(record[i]) + "," : std::to_string(record[i]);
    update += cur;
  }
  trt_sol_cnt << update << std::endl;
}

void DiaWorld::AverageTraitError(size_t up) {             ///< Average error on a trait, per population, per update
  // Hold solution counts
  emp::vector<double> record;
  record.resize(config.K_TRAITS(), 0.0);
  // Iterate through the internal values
  for(size_t k = 0; k < config.K_TRAITS(); k++) {
    // Iterate through the pop per internal val
    for(size_t i = 0; i < pop.size(); i++) {
      // Get org and its score on the ith internal k val
      DiaOrg & org = *pop[i];
      double score = org.GetScore(k);
      // Add to records
      record[k] += score;
    }
  }

  // Divide all scores by the pop size!
  for(size_t i = 0; i < config.K_TRAITS(); i++) { record[i] /= (double) config.POP_SIZE();}

  // Write all data to the csv
  std::string update = std::to_string(up) + ",";
  for(size_t i = 0; i < config.K_TRAITS(); i++) {
    std::string cur = (i < config.K_TRAITS()-1) ? std::to_string(record[i]) + "," : std::to_string(record[i]);
    update += cur;
  }

  trt_avg_err << update << std::endl;
}

void DiaWorld::MinimumTraitError(size_t up) {             ///< Get best error per trait
  // Hold solution counts
  emp::vector<double> record;
  record.resize(config.K_TRAITS(), 100000.0);
  // Iterate through the internal values
  for(size_t k = 0; k < config.K_TRAITS(); k++) {
    // Iterate through the pop per internal val
    for(size_t i = 0; i < pop.size(); i++) {
      // Get org and its score on the ith internal k val
      DiaOrg & org = *pop[i];
      double score = org.GetScore(k);
      // Check if we find a smaller error!
      if(score < record[k]) {
        record[k] = score;
      }
    }
  }

  // Write all data to the csv
  std::string update = std::to_string(up) + ",";
  for(size_t i = 0; i < config.K_TRAITS(); i++) {
    std::string cur = (i < config.K_TRAITS()-1) ? std::to_string(record[i]) + "," : std::to_string(record[i]);
    update += cur;
  }

  trt_min_err << update << std::endl;
}

void DiaWorld::AveragePopError(size_t up) {             ///< Average error for a population per update
  // Hold population error
  double error = 0.0;

  // Iterate through the pop
  for(size_t i = 0; i < pop.size(); i++) {
    DiaOrg & org = *pop[i];
    error += org.GetTotalScore();
  }

  error /= (double) pop.size();

  pop_avg_err << std::to_string(up) + "," + std::to_string(error) << std::endl;
}


#endif