/// This is the world for DiaOrgs

///< Defnining name
#ifndef DIA_WORLD_H
#define DIA_WORLD_H

///< Standard headers
#include <functional>
#include <utility>
#include <cmath>
#include <fstream>

///< Empirical headers
#include "Evolve/World.h"
#include "base/vector.h"

///< Experiment headers
#include "config.h"
#include "DiagnosticOrg.h"

///< Custom naming
using ids_t = emp::vector<size_t>;
using fit_fun = std::function<double(DiaOrg &)>;
using eva_fun = std::function<void(DiaOrg &)> ;
using sel_fun = std::function<ids_t()>;
using tar_t = emp::vector<double>;

class DiaWorld : public emp::World<DiaOrg> {

  private:
    DiaWorldConfig & config;      ///< Experiments configurations
    fit_fun fitness;              ///< Experiemnt fitness function
    sel_fun select;               ///< Experiment selection fucntion
    eva_fun evaluate;             ///< Experiment evaluation function
    ids_t pop_ids;                ///< Population IDs for randomly picking from the world
    tar_t target;                 ///< Targets that organisms are trying to reach
    std::ofstream sol_cnt;        ///< Track data regarding solution count
    std::ofstream avg_err;        ///< Track the average error per internal val per update
    std::ofstream min_err;        ///> Track the minimum error per internal val per update

  public:
    DiaWorld(DiaWorldConfig & _config) : config(_config) {      ///< Constructor
      // Set requested selection and fitness functions
      InitialSetup();
      // Populate the pop_ids
      for(size_t i = 0; i < config.POP_SIZE(); i++) { pop_ids.push_back(i); }
      // Initialize pointer
      random_ptr = emp::NewPtr<emp::Random>(config.SEED());

      if(!config.MULTIOBJECTIVE()) {
        std::cerr << "MULTIOBJECTIVE: False" << std::endl;
        target.resize(config.K_INTERNAL(), config.TARGET());
      }
      else {

      }
    }

    ~DiaWorld() {
      sol_cnt.close();
      avg_err.close();
      min_err.close();
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

    void LexicaseFitnessFun();        ///< Set fitness function for Lexicase
    void LexicaseSelection();         ///< Set Lexicase Selection Algorithm
    void LexicaseExploit();           ///< Set fitness function as Exploitation
    void LexicaseStructExploit();     ///< Set fitness function as Structured Exploitation


    /* Functions for Evaluation set up */

    void EvalExploit();               ///< Evaluate organisms with exploitation metric
    void EvalStructExploit();         ///< Evalate organisms with structured exploitation metric


    /* Functions ran during experiment */

    void Evaluate();                ///< Evaluate all orgs on individual test cases!
    ids_t Selection();              ///< Call when its time to select parents
    size_t Mutate();                ///< Call when we need to mutate an organism
    void Births(ids_t parents);                  ///< Call when its time to produce offspring


    /* Functions for gathering data */

    void SetCSVHeaders();                   ///< Set up all headers of csv files!
    void GatherData(size_t up);             ///< Will call all other functions to gather data
    void CountSolution(size_t up);          ///< Given some threshold, how many solutions do we have?
    void AverageError(size_t up);           ///< Average error on a k-val, per population, per update
    void MinimumError(size_t up);           ///< Get best error per k-val
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
    emp_assert(genome.size() == config.K_INTERNAL(), genome.size());

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
      org.Reset(config.K_INTERNAL());
    }
  });
}

void DiaWorld::InitializeWorld() {  ///< Set initial population of orgs
  // Fill the workd with requested population size!
  DiaOrg org(config.K_INTERNAL());
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
        double score = fitness(GetOrg(i));

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
  fitness = [this] (DiaOrg & org) {
    // Return the sum of errors!
    return org.GetTotalScore();
  };
}

void DiaWorld::TournamentStructExploit() {    ///< Set fitness function as Structured Exploitation
  std::cerr << "Structured Exploitation" << std::endl;
  fitness = [this] (DiaOrg & org) {
    // Return the sum of errors!
    return org.GetTotalScore();
  };
}


/* Functions for Lexicase Selection set up */

void DiaWorld::LexicaseFitnessFun() {        ///< Set fitness function for Lexicase
}

void DiaWorld::LexicaseSelection() {         ///< Set Lexicase Selection Algorithm
}

void DiaWorld::LexicaseExploit() {           ///< Set fitness function as Exploitation
}

void DiaWorld::LexicaseStructExploit() {     ///< Set fitness function as Structured Exploitation
}


/* Functions ran during experiment */

void DiaWorld::Evaluate() {          ///< Evaluate all orgs on individual test cases!
  //Make sure pop is the correct size
  emp_assert(pop.size() == config.POP_SIZE(), pop.size());
  emp_assert(target.size() == config.K_INTERNAL(), target.size());

  // Loop through world and score orgs
  for(size_t pos = 0; pos < pop.size(); pos++) {
    // Get org and calculate score
    auto & org = *pop[pos];
    org.Reset(config.K_INTERNAL());

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
  // Initialize data trackers
  sol_cnt.open("sol_cnt.csv");
  avg_err.open("avg_err.csv");
  min_err.open("min_err.csv");

  // Create header
  std::string head = "Update,";
  for(size_t i = 0; i < config.K_INTERNAL(); i++) {
    std::string cur;
    if(i < config.K_INTERNAL()-1) {
      cur = "val-" + std::to_string(i) + ",";
      head += cur;
    }
    else {
      cur = "val-" + std::to_string(i) + "\n";
      head += cur;
    }
  }

  sol_cnt << head;
  avg_err << head;
  min_err << head;
}

void DiaWorld::GatherData(size_t up) {               ///< Will call all other functions to gather data
  CountSolution(up);
  AverageError(up);
  MinimumError(up);
}

void DiaWorld::CountSolution(size_t up) {            ///< Given some threshold, how many solutions do we have?
  // Hold solution counts
  emp::vector<size_t> record;
  record.resize(config.K_INTERNAL(), 0);
  // Iterate through the internal values
  for(size_t k = 0; k < config.K_INTERNAL(); k++) {
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

  // Write the data to the csv file!
  std::string update = std::to_string(up) + ",";
  for(size_t i = 0; i < config.K_INTERNAL(); i++) {
    std::string cur;
    if(i < config.K_INTERNAL()-1) {
      cur = std::to_string(record[i]) + ",";
      update += cur;
    }
    else {
      cur = std::to_string(record[i]) + "\n";
      update += cur;
    }
  }

  sol_cnt << update;
}

void DiaWorld::AverageError(size_t up) {             ///< Average error on a k-val, per population, per update
  // Hold solution counts
  emp::vector<double> record;
  record.resize(config.K_INTERNAL(), 0.0);
  // Iterate through the internal values
  for(size_t k = 0; k < config.K_INTERNAL(); k++) {
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
  for(size_t i = 0; i < config.K_INTERNAL(); i++) { record[i] /= (double) config.POP_SIZE();}

  // Write all data to the csv
  std::string update = std::to_string(up) + ",";
  for(size_t i = 0; i < config.K_INTERNAL(); i++) {
    std::string cur;
    if(i < config.K_INTERNAL()-1) {
      cur = std::to_string(record[i]) + ",";
      update += cur;
    }
    else {
      cur = std::to_string(record[i]) + "\n";
      update += cur;
    }
  }

  avg_err << update;
}

void DiaWorld::MinimumError(size_t up) {             ///< Get best error per k-val
  // Hold solution counts
  emp::vector<double> record;
  record.resize(config.K_INTERNAL(), 100000.0);
  // Iterate through the internal values
  for(size_t k = 0; k < config.K_INTERNAL(); k++) {
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
  for(size_t i = 0; i < config.K_INTERNAL(); i++) {
    std::string cur;
    if(i < config.K_INTERNAL()-1) {
      cur = std::to_string(record[i]) + ",";
      update += cur;
    }
    else {
      cur = std::to_string(record[i]) + "\n";
      update += cur;
    }
  }

  min_err << update;
}
#endif