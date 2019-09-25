/// This is the world for DiaOrgs

///< Defnining name
#ifndef DIA_WORLD_H
#define DIA_WORLD_H

///< Standard headers
#include <functional>
#include <utility>
#include <cmath>

///< Empirical headers
#include "Evolve/World.h"
#include "base/vector.h"

///< Experiment headers
#include "config.h"
#include "DiagnosticOrg.h"

///< Custom naming
using ids_t = emp::vector<size_t>;
using fit_fun = std::function<double(DiaOrg &)>; 
using sel_fun = std::function<ids_t()>;
using tar_t = emp::vector<double>;

class DiaWorld : public emp::World<DiaOrg> {

  private:
    DiaWorldConfig & config;      ///< Experiments configurations
    fit_fun fitness;              ///< Experiemnt fitness function 
    sel_fun select;               ///< Experiment selection fucntion
    ids_t pop_ids;                ///< Population IDs for randomly picking from the world
    tar_t target;                 ///< Targets that organisms are trying to reach

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

    /* Functions for initial experiment set up */
    
    void InitialSetup();            ///< Do all the initial set up    
    void SetOnUpdate();             ///< Set up world configurations        
    void SetMutations();            ///< Set up the mutations parameters
    void SetSelectionFun();         ///< Set up the selection function
    void SetOnOffspringReady();     ///< Set up the OnOffspringReady function
    void InitializeWorld();         ///< Set initial population of orgs

    ///< Tournament Selection Set Up
    void TournamentFitnessFun();        ///< Set fitness function for Tournament 
    void TournamentSelection();         ///< Set Tournament Selection Algorithm
    void TournamentExploit();           ///< Set fitness function as Exploitation


    /* Functions ran during experiment */

    void Evaluate();                ///< Evaluate all orgs on individual test cases!
    ids_t Selection();              ///< Call when its time to select parents
    size_t Mutate();                ///< Call when we need to mutate an organism
    void Births(ids_t parents);                  ///< Call when its time to produce offspring


    /* Functions for gathering data */

    void GatherData();             ///< Will call all other functions to gather data
    void CountSolution();          ///< Given some threshold, how many solutions do we have?
    void AverageError();           ///< Average error on a k-val, per population, per update
    void MinimumError();           ///< Get best error per k-val
};

/* Functions for initial experiment set up */

void DiaWorld::InitialSetup() {     ///< Do all the initial set up
  Reset();
  SetPopStruct_Mixed(true);
  SetMutations();
  SetSelectionFun();
  SetOnOffspringReady();
  InitializeWorld();
  SetOnUpdate();
}  

void DiaWorld::SetOnUpdate() {      ///< Set up world configurations
  OnUpdate([this](size_t) {
    // Evaluate all organisms
    Evaluate();
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
    // Check that genome is correct length
    emp_assert(genome.size() == config.K_INTERNAL(), genome.size());

    // Loop through and apply mutations if applicable
    for(size_t i = 0; i < genome.size(); i++) {
      // Can we do a mutation?
      if(random_ptr->P(config.MUTATE_VAL())) {
        // Apply mutation from normal distribution
        double evo = random_ptr->GetRandNormal(config.MEAN(), config.STD());
        genome[i] += evo;
      }
    }

    return 1;
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

///< Tournament Selection Set Up
void DiaWorld::TournamentFitnessFun() {    
  // Set up the cases to set the fitness fuction
  switch (config.DIAGNOSTIC()) {
    case 0:  // Exploitation
      TournamentExploit();
      break;

    case 1:  // Structured Exploitation
      /* code */
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
  emp_assert(config.TOUR_SIZE() < config.POP_SIZE(), config.TOUR_SIZE());
  // Setting up tournament selection algorithm
  select = [this] () {
    // Holds all ids of parents selected for reproduction
    ids_t parents;

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
      // Select a parent and store for reproduction
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
    return org.GetTotal();
  };
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

    // Loop through ground truth vector and score!
    for(size_t i = 0; i < target.size(); i++) {
      // Calculate individual values
      org.CalculateScore(i, target[i]);
    }
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


/* Functions for gathering data */
void DiaWorld::GatherData() {               ///< Will call all other functions to gather data

}

void DiaWorld::CountSolution() {            ///< Given some threshold, how many solutions do we have?

}

void DiaWorld::AverageError() {             ///< Average error on a k-val, per population, per update

}

void DiaWorld::MinimumError() {             ///< Get best error per k-val

}
#endif