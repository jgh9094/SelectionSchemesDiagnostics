/// This is the world for DiaOrgs

// Hello World

/* TODO SECTION
 * [X]: Finished coding >:D
 * [P]: Process of coding >:|
 * [?]: Need to figure out still :?
 * []: Nothing done yet :)
 *
 * SELECTIONS:
 * [X]: Tournament
 * [X]: Lexicase
 * [X]: Drift
 * [X]: Cohort Lexicase
 * [X]: Down Sampled Lexicase
 * []: EcoEA
 * []: Roulette
 *
 * DIAGNOSTICS:
 * [X]: Exploitation
 * [X]: Structured Exploitation
 * [P]: Contradictory K Traits Ecologoy
 * []: Weak Ecology
 * []: Specialist
 * []: Good & Bad Hints
 * []: Bias
 * []: Deceptive
 * []: Overfitting With Noise
 * [?]: Overfitting With Smoothness
 * []: Exploitation
 *
 * DATA Analysis
 * [X]: Triat Average Error
 * [X]: Trait Min Error
 * [X]: Trait Solution Count
 * [X]: Population Average Error
 * [X]: Unique Genotypes
*/

// Hello world

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

template <typename PHEN_TYPE>
struct pheno_info { /// Track information related to the mutational landscape
  /// Maps a string representing a type of mutation to a count representing
  /// the number of that type of mutation that occured to bring about this taxon.
  using phen_t = PHEN_TYPE;
  using has_phen_t = std::true_type;
  using has_mutations_t = std::false_type;
  using has_fitness_t = std::true_type;
  // using has_phenotype_t = true;

  emp::DataNode<double, emp::data::Current, emp::data::Range> fitness; /// This taxon's fitness (for assessing deleterious mutational steps)
  PHEN_TYPE phenotype; /// This taxon's phenotype (for assessing phenotypic change)

  const PHEN_TYPE & GetPhenotype() const {
    return phenotype;
  }

  const double GetFitness() const {
    return fitness.GetMean();
  }

  void RecordFitness(double fit) {
    fitness.Add(fit);
  }

  void RecordPhenotype(PHEN_TYPE phen) {
    phenotype = phen;
  }

};

class DiaWorld : public emp::World<DiaOrg> {

  ///< Custom naming
  public:

    using ids_t = emp::vector<size_t>;
    using fit_agg_t = std::function<double(DiaOrg &)>;
    using fit_lex_t = std::function<double(DiaOrg &, size_t)>;
    using eva_fun = std::function<void(DiaOrg &)> ;
    using sel_fun = std::function<ids_t()>;
    using tar_t = emp::vector<double>;
    using cls_t = emp::vector<ids_t>;

    ///< Data tracking with MONSTER Branch
    // using phenotype_t = DiaOrg;
    using systematics_t = emp::Systematics<DiaOrg, DiaOrg::genome_t, pheno_info<typename DiaOrg::phenot_t>>;
    using taxon_t = typename systematics_t::taxon_t;

  private:

    /* Experiment Set Up */
    DiaWorldConfig & config;          ///< Experiments configurations
    fit_agg_t fitness_agg;            ///< Variable to return aggregate errors
    fit_lex_t fitness_lex;            ///< Variable to fitness for a specific case
    sel_fun select;                   ///< Experiment selection fucntion
    eva_fun evaluate;                 ///< Experiment evaluation function

    /* Population, Org, and Traits */
    ids_t pop_ids;                      ///< Population IDs for randomly picking from the world
    ids_t trt_ids;                      ///< Vector holding ids for each trait we are evaluating
    tar_t target;                       ///< Targets that organisms are trying to reach

    /* Cohort Lexicase */
    cls_t pop_coh;                      ///< Hold the organism cohorts
    cls_t trt_coh;                      ///< Hold the trait cohorts
    int coh_pop_num = -1;               ///< Number of popluation cohorts
    int coh_pop_sze = -1;               ///< Size of each population cohort
    int coh_trt_num = -1;               ///< Number of trait cohorts
    int coh_trt_sze = -1;               ///< Size of each trait cohort

    /* DownSample Lexicase */
    int sample_size = -1;                 ///< How many test cases do we have?

    /* Data Tracking */
    std::fstream trt_sol_cnt;         ///< Track number of solutions for a trait per update
    std::fstream trt_avg_err;         ///< Track the average error for a trait per update
    std::fstream trt_min_err;         ///< Track the minimum error for a trait  per update
    std::fstream pop_stats;         ///< Track the average error for the population each generation
    emp::Ptr<systematics_t> sys_ptr; ///< Short cut to correctly-typed systematics manager. Base class will be responsible for memory management.


  public:

    DiaWorld(DiaWorldConfig & _config) : config(_config) {      ///< Constructor
      // Initialize the vector to poplulation pop_ids
      for(size_t i = 0; i < config.POP_SIZE(); i++) {pop_ids.push_back(i);}
      // Initialze the  vector t0 traits trt_ids
      for(size_t i = 0; i < config.K_TRAITS(); i++) {trt_ids.push_back(i);}
      // Initialize pointer
      random_ptr = emp::NewPtr<emp::Random>(config.SEED());
      // If not multiobjective, then we can set all traits to the same value
      if(!config.MULTIOBJECTIVE()) {
        std::cerr << "MULTIOBJECTIVE: False" << std::endl;
        target.resize(config.K_TRAITS(), config.TARGET());
      }
      else {

      }

      // Initialize systematic tracking
      sys_ptr = emp::NewPtr<systematics_t>([](const DiaOrg & o) { return o.GetGenome(); });

      sys_ptr->AddSnapshotFun([](const taxon_t & taxon) {
        return emp::to_string(taxon.GetData().GetFitness());
      }, "fitness", "Taxon fitness");

      sys_ptr->AddSnapshotFun([](const taxon_t & taxon) {
        return emp::ToString(taxon.GetData().GetPhenotype());
      }, "phenotype", "Taxon Phenotype");

      sys_ptr->AddSnapshotFun([](const taxon_t & taxon) {
        return emp::ToString(taxon.GetInfo());
      }, "genotype", "Taxon Genotype");

      AddSystematics(sys_ptr);
      SetupSystematicsFile(0, config.OUTPUT_DIR() + "systematics.csv").SetTimingRepeat(config.PRINT_INTERVAL());

      // Set requested selection and fitness functions
      InitialSetup();
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
    void SetSystematics();          ///< Set systematic manager


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


    /* Functions for Drift Selction set up */

    void DriftFitnessFun();
    void DriftSelection();


    /* Functions for Drift Selction set up */

    void CohortLexicaseFitnessFun();          ///< Set fitness function for Lexicase
    void CohortLexicaseSelection();           ///< Set Lexicase Selection Algorithm
    void CreateCohortsCLS();                  ///< Create the cohorts for both population and traits
    void CohortLexicaseSymmetry();            ///< Checks if the cohort proportions work out
    void CohortLexicaseExploit();             ///< Set fitness function as Exploitation
    void CohortLexicaseStructExploit();       ///< Set fitness function as Structured Exploitation

    /* Functions for DownSample Lexicase */

    void DownSampleLexicaseFitnessFun();           ///< Set fitness function for DownSample Lexicase
    void DownSampleLexicaseSelection();            ///< Set Lexicase Selection Algorithm
    void DownSampleLexicaseValidity();             ///< Checks if the cohort proportions work out
    void DownSampleLexicaseExploit();              ///< Set fitness function as Exploitation
    void DownSampleLexicaseStructExploit();        ///< Set fitness function as Structured Exploitation


    /* Functions for Evaluation set up */

    void EvalExploit();               ///< Evaluate organisms with exploitation
    void EvalStructExploit();         ///< Evalate organisms with structured exploitation


    /* Functions ran during experiment */

    void Evaluate();                ///< Evaluate all orgs on individual test cases!
    ids_t Selection();              ///< Call when its time to select parents
    size_t Mutate();                ///< Call when we need to mutate an organism
    void Births(const ids_t & parents);                  ///< Call when its time to produce offspring


    /* Functions for gathering data */

    void SetCSVHeaders();                        ///< Set up all headers of csv files!
    void GatherData(size_t up);                  ///< Will call all other functions to gather data
    void CountTraitSolution(size_t up);          ///< Given some threshold, how many solutions do we have?
    void AverageTraitError(size_t up);           ///< Average error on a trait, per population, per update
    void MinimumTraitError(size_t up);           ///< Get best error per trait
    void PopulationData(size_t up);              ///< Call functions that return single value pop data
    double AveragePopError();                    ///< Average error for a population per update
    double MinPopError();                        ///< Total error for the best performing org in a generation
    size_t UniqueGenomes();                      ///< Number of Unique genomes per update


    void SnapshotPhylogony();

    /* DEBUGGINGGGGGGGGGG */

    void PrintCohorts();      ///< Will print all of the cohorts
};

/* Functions for initial experiment set up */

///< Do all the initial set up
void DiaWorld::InitialSetup() {
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

///< Set up world configurations
void DiaWorld::SetOnUpdate() {
  OnUpdate([this](size_t) {
    // Evaluate all organisms
    Evaluate();
    // Gather relavant data!
    GatherData(GetUpdate());
    // Take the snapshot
    if(GetUpdate() % config.SNAP_INTERVAL() == 0 || GetUpdate() == config.MAX_GENS()-1) {
      SnapshotPhylogony();
    }
    // Select parents for next gen
    auto parents = Selection();
    // Give birth to the next gen & mutate
    Births(parents);
  });
}

///< Set up the mutations parameters
void DiaWorld::SetMutations() {
  // Set up the worlds mutation function!
  SetMutFun([this](DiaOrg & org, emp::Random & random) {
    // Get org genome!
    auto & genome = org.GetGenome();
    size_t cnt = 0;
    // Check that genome is correct length
    emp_assert(genome.size() == config.K_TRAITS(), genome.size());

    // Loop through and apply mutations if applicable
    for(size_t i = 0; i < genome.size(); i++) {
      // Can we do a point offset mutation?
      if(random_ptr->P(config.MUTATE_VAL())) {
        // Get mutation and see if we need to cap :P
        double evo = random_ptr->GetRandNormal(config.MEAN(), config.STD());

        // Check if we are over
        if(genome[i] + evo > config.TARGET()) {
          genome[i] = config.TARGET();
        }
        // Check if we are under
        else if (genome[i] + evo < 0.0) {
          genome[i] = 0.0;
        }
        else {
          genome[i] += evo;
        }
        cnt += 1;
      }
    }

    return cnt;
  });
}

///< Set up the selection function
void DiaWorld::SetSelectionFun() {
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
    std::cerr << "SELECTION: "; DriftSelection();
    std::cerr << "DIAGNOSTIC: "; DriftFitnessFun();
    break;

  case 3: // Cohort Lexicase
    // Check if conditions are met
    CohortLexicaseSymmetry();
    std::cerr << "SELECTION: "; CohortLexicaseSelection();
    std::cerr << "DIAGNOSTIC: "; CohortLexicaseFitnessFun();
    break;

  case 4: // Cohort Lexicase
    // Check if conditions are met
    DownSampleLexicaseValidity();
    std::cerr << "SELECTION: "; DownSampleLexicaseSelection();
    std::cerr << "DIAGNOSTIC: "; DownSampleLexicaseFitnessFun();
    break;

  default:
    std::cerr << "SELECTED INVALID SELECTION SCHEME" << std::endl;
    exit(-1);
    break;
  }
}

///< Set up the Evaluation function
void DiaWorld::SetEvaluationFun() {
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

///< Set up the OnOffspringReady function
void DiaWorld::SetOnOffspringReady() {
  // Create lambda function
  OnOffspringReady([this](DiaOrg & org, size_t parent_pos) {
    // Mutate organism if possible
    if(config.MUTATE()) {
      DoMutationsOrg(org);
      org.Reset(config.K_TRAITS());
    }
  });
}

///< Set initial population of orgs
void DiaWorld::InitializeWorld() {
  // Fill the workd with requested population size!
  DiaOrg org(config.K_TRAITS());
  Inject(org.GetGenome(), config.POP_SIZE());
}


/* Functions for Tournament Selection set up */

///< Set fitness function for Tournament
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

///< Set Tournament Selection Algorithm
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

    emp_assert(parents.size() == config.POP_SIZE(), parents.size());
    return parents;
  };
}

///< Set fitness function as Exploitation
void DiaWorld::TournamentExploit(){
  std::cerr << "Standard Exploitation" << std::endl;
  fitness_agg = [this] (DiaOrg & org) {
    // Return the sum of errors!
    return org.GetTotalScore();
  };
}

///< Set fitness function as Structured Exploitation
void DiaWorld::TournamentStructExploit() {
  std::cerr << "Structured Exploitation" << std::endl;
  fitness_agg = [this] (DiaOrg & org) {
    // Return the sum of errors!
    return org.GetTotalScore();
  };
}


/* Functions for Lexicase Selection set up */

///< Set fitness function for Lexicase
void DiaWorld::LexicaseFitnessFun() {
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

///< Set Lexicase Selection Algorithm
void DiaWorld::LexicaseSelection() {
  // What are we doing?
  std::cerr << "Lexicase Selection" << std::endl;

  // Check if we even have test cases lol
  emp_assert(trt_ids.size() > 0, trt_ids.size());
  emp_assert(pop_ids.size() == config.POP_SIZE(), pop_ids.size());

  // Set the selction lambda
  select = [this] () {
    // Will hold all the parent ids for repreduction
    ids_t parents;

    // Keep looping through until we have enough parents
    while(parents.size() != config.POP_SIZE()) {
      // Shuffle test cases
      emp::Shuffle(*random_ptr, trt_ids);
      // this list of parents will be used by lexicase to single out a parent
      ids_t round_winners = pop_ids;
      // Get the winning parent
      size_t winner = LexicaseWinner(round_winners, trt_ids);
      // Add parent to the next gen
      parents.push_back(winner);
    }

    emp_assert(parents.size() == config.POP_SIZE(), parents.size());
    return parents;
  };
}

///< This function will take in an initial population and trim it down
size_t DiaWorld::LexicaseWinner(ids_t & round_pop, ids_t const & test_cases) {
  // Will hold all the scores with associated ids
  std::map<double, ids_t> scores;
  // Position trackers for test cases
  size_t tc = 0;

  // Loop through all the test cases until we have a single winner or
  // run out of test cases to use! Will randomly select winner if more than one
  // at the end of the round.
  while(tc < test_cases.size() && round_pop.size() != 1) {
    // Clear scores for this round!
    scores.clear();

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

///< Set fitness function as Exploitation
void DiaWorld::LexicaseExploit() {
  std::cerr << "Exploitation" << std::endl;
  fitness_lex = [this] (DiaOrg & org, size_t i) {
    return org.GetScore(i);
  };
}

///< Set fitness function as Structured Exploitation
void DiaWorld::LexicaseStructExploit() {
  std::cerr << "Structured Exploitation" << std::endl;
  fitness_lex = [this] (DiaOrg & org, size_t i) {
    return org.GetScore(i);
  };
}


/* Function for Drift Selction set up */

///< Drift has no fitness function to optimize!
void DiaWorld::DriftFitnessFun() {
  /* Fitness does not matter for this case
     but function if user would like the user
     would like to track additional things! */

     std::cerr << "Drift Requires No Fitness!" << std::endl;
}

///< Set select lambda with drift selection
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

    emp_assert(parents.size() == config.POP_SIZE(), parents.size());
    return parents;
  };

}


/* Function for Cohort Lexicase Selction set up */

///< Set fitness function for Lexicase
void DiaWorld::CohortLexicaseFitnessFun() {
  // Set up the cases to set the fitness fuction
  switch (config.DIAGNOSTIC()) {
    case 0:  // Exploitation
      CohortLexicaseExploit();
      break;

    case 1:  // Structured Exploitation
      CohortLexicaseStructExploit();
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

///< Set Lexicase Selection Algorithm
void DiaWorld::CohortLexicaseSelection() {
  // What are we doing?
  std::cerr << "Cohort Lexicase Selection" << std::endl;

  // Check if we even have test cases lol
  emp_assert(trt_ids.size() > 0, trt_ids.size());
  emp_assert(pop_ids.size() == config.POP_SIZE(), pop_ids.size());

  // Set the selection lambda
  select = [this] () {
    // Create a new cohort for pop and traits
    CreateCohortsCLS();

    // Will hold the parents ids for reproduction
    ids_t parents;

    // Go through each cohort in the pop_coh and trt_coh respectively
    for(int c = 0; c < coh_pop_num; c++) {
      // Get a cohort of test cases
      ids_t traits = trt_coh[c];
      // Loop coh_pop_sze time to get the number of parents for a pop_coh cohort
      for(int i = 0; i < coh_pop_sze ; i++) {
        // Shuffle the traits
        emp::Shuffle(*random_ptr, traits);
        // Get list of org ids in specific cohort
        ids_t round_winners = pop_coh[c];
        // Get the winning parent
        size_t winner = LexicaseWinner(round_winners, traits);
        // Store winning parent ID
        parents.push_back(winner);
      }
    }

    emp_assert(parents.size() == config.POP_SIZE(), parents.size());
    return parents;
  };

}

///< Create cohorts for lexicase selection
void DiaWorld::CreateCohortsCLS() {
  // Make sure that we have everything set up accordingly
  emp_assert(coh_pop_num != -1, coh_pop_num);
  emp_assert(coh_pop_sze != -1, coh_pop_sze);
  emp_assert(coh_trt_num != -1, coh_trt_num);
  emp_assert(coh_trt_sze != -1, coh_trt_sze);

  // Shuffle the population ids
  ids_t pop_order = pop_ids;
  emp::Shuffle(*random_ptr, pop_order);

  // Now place them in the appropiate cohort
  for(size_t i = 0; i < pop_order.size(); i++) {
    // What cohort and position are we placing in?
    size_t coh = i / coh_pop_sze;
    size_t pos = i % coh_pop_sze;
    // Place in the cohort
    pop_coh[coh][pos] = pop_order[i];
  }

  // Shuffle the trait ids
  ids_t trt_order = trt_ids;
  emp::Shuffle(*random_ptr, trt_order);

  // Now place them in the appropiate cohort
  for(size_t i = 0; i < trt_order.size(); ++i) {
    // What cohort and position are placing in?
    size_t coh = i / coh_trt_sze;
    size_t pos = i % coh_trt_sze;
    // Place in cohort
    trt_coh[coh][pos] = trt_order[i];
  }
}

///< Checks if the cohort proportions work out
void DiaWorld::CohortLexicaseSymmetry() {
  // Check some math real quick for the population
  int coh_size = config.POP_SIZE() * config.CLS_POP_PROP();
  int coh_numb = config.POP_SIZE() / coh_size;
  size_t total = coh_size * coh_numb;
  if(total != config.POP_SIZE()) {
    std::cerr << "coh_size=" << coh_size << std::endl;
    std::cerr << "coh_numb=" << coh_numb << std::endl;
    std::cerr << "total=" << total << std::endl;
    std::cerr << "COHORT LEXICASE PROPORTION MUST ALLOW FOR SYMMETRY BETWEEN POPULATION COHORTS" << std::endl;
    exit(-1);
  }

  // Check some math real quick for the traits
  int trt_size = config.K_TRAITS() * config.CLS_TRT_PROP();
  int trt_numb = config.K_TRAITS() / trt_size;
  size_t totall = trt_size * trt_numb;
  if(totall != config.K_TRAITS()) {
    std::cerr << "trt_size=" << trt_size << std::endl;
    std::cerr << "trt_numb=" << trt_numb << std::endl;
    std::cerr << "totall=" << totall << std::endl;
    std::cerr << "COHORT LEXICASE PROPORTION MUST ALLOW FOR SYMMETRY BETWEEN TRAIT COHORTS" << std::endl;
    exit(-1);
  }

  // Make sure there an equal number of cohorts between traits and population
  if(coh_numb != trt_numb) {
    std::cerr << "trt_numb=" << trt_numb << std::endl;
    std::cerr << "coh_numb=" << coh_numb << std::endl;
    std::cerr << "COHORT LEXICASE NUMBER OF COHORTS MUST MATCH BETWEEN TRAITS AND POPULATION" << std::endl;
    exit(-1);
  }

  // Set all the numbers so we can generate the cohorts!
  coh_pop_num = coh_numb;
  coh_pop_sze = coh_size;
  coh_trt_num = trt_numb;
  coh_trt_sze = trt_size;

  // Set all lengths for cohorts
  pop_coh.resize(coh_pop_num);
  for(auto & v : pop_coh) {v.resize(coh_pop_sze);}
  trt_coh.resize(coh_trt_num);
  for(auto & v : trt_coh) {v.resize(coh_trt_sze);}

  std::cerr << "COHORT LEXICASE SYMMETRY MET!" << std::endl;
}

///< Set fitness function as Exploitation
void DiaWorld::CohortLexicaseExploit() {
  std::cerr << "Exploitation" << std::endl;
  fitness_lex = [this] (DiaOrg & org, size_t i) {
    return org.GetScore(i);
  };
}

///< Set fitness function as Structured Exploitation
void DiaWorld::CohortLexicaseStructExploit() {
  std::cerr << "Structured Exploitation" << std::endl;
  fitness_lex = [this] (DiaOrg & org, size_t i) {
    return org.GetScore(i);
  };
}


/* Functions for DownSample Lexicase */

///< Set fitness function for DownSample Lexicase
void DiaWorld::DownSampleLexicaseFitnessFun() {
  // Set up the cases to set the fitness fuction
  switch (config.DIAGNOSTIC()) {
    case 0:  // Exploitation
      DownSampleLexicaseExploit();
      break;

    case 1:  // Structured Exploitation
      DownSampleLexicaseStructExploit();
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

///< Set Lexicase Selection Algorithm
void DiaWorld::DownSampleLexicaseSelection() {
  // What are we doing?
  std::cerr << "DownSample Lexicase Selection" << std::endl;

  // Check if we even have test cases lol
  emp_assert(trt_ids.size() > 0, trt_ids.size());
  emp_assert(pop_ids.size() == config.POP_SIZE(), pop_ids.size());
  emp_assert(sample_size != -1, sample_size);

  // Set the selection lambda
  select = [this] () {
    // Will hold all the parents
    ids_t parents;
    // Get a fresh sample of test cases
    ids_t trt_ids = emp::Choose(*random_ptr, config.K_TRAITS(), sample_size);

    // Loop until we find the the correct amount of parents
    while(parents.size() != config.POP_SIZE()) {
      // Shuffle the traits
      emp::Shuffle(*random_ptr, trt_ids);
      // Get list of current pop to trim down
      ids_t round_winners = pop_ids;
      // Get the winning parent
      size_t winner = LexicaseWinner(round_winners, trt_ids);
      // Add parent to the next gen
      parents.push_back(winner);
    }

    // Check if we even have enough
    emp_assert(parents.size() == config.POP_SIZE(), parents.size());
    return parents;
  };
}

///< Checks if the cohort proportions work out
void DiaWorld::DownSampleLexicaseValidity() {
  // We need to see if the proportion is even valid
  if(config.DLS_TRT_PROP() < 0 || config.DLS_TRT_PROP() > 1) {
    std::cerr << "config.DLS_TRT_PROP() IS NOT IN RANGE OF (0,1]" << std::endl;
    exit(-1);
  }

  sample_size = config.DLS_TRT_PROP() * (double) config.K_TRAITS();
  std::cerr << "sample_size=" << sample_size << std::endl;
}

///< Set fitness function as Exploitation
void DiaWorld::DownSampleLexicaseExploit() {
  std::cerr << "Exploitation" << std::endl;
  fitness_lex = [this] (DiaOrg & org, size_t i) {
    return org.GetScore(i);
  };
}

///< Set fitness function as Structured Exploitation
void DiaWorld::DownSampleLexicaseStructExploit() {
  std::cerr << "Structured Exploitation" << std::endl;
  fitness_lex = [this] (DiaOrg & org, size_t i) {
    return org.GetScore(i);
  };
}


/* Functions ran during experiment */

///< Evaluate all orgs on individual test cases!
void DiaWorld::Evaluate() {
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

    // systematic stuff
    emp::Ptr<taxon_t> taxon = sys_ptr->GetTaxonAt(pos);
    taxon->GetData().RecordFitness(org.GetTotalScore());
    taxon->GetData().RecordPhenotype(org.GetPheno());
  }
}

///< Call when its time to select parents
DiaWorld::ids_t DiaWorld::Selection() {
  return select();
}

///< Call when its time to produce offsprings from parents
void DiaWorld::Births(const ids_t & parents) {
  // Go through the parent ids and give birth!
  for(auto & id : parents) {
    DoBirth(GetGenomeAt(id), id);
  }
}


/* Functions for Evaluation set up */

///< Evaluate organisms with exploitation metrics
void DiaWorld::EvalExploit() {
  std::cerr << "Exploitation" << std::endl;
  // Set the evaluation function
  evaluate = [this] (DiaOrg & org) {
    org.ExploitError(target);
  };
}

///< Evalate organisms with structured exploitation metric
void DiaWorld::EvalStructExploit() {
  std::cerr << "Structured Exploitation" << std::endl;
  // Set up structured exploitation function
  evaluate = [this] (DiaOrg & org) {
    org.StructExploitError(target);
  };
}


/* Functions for gathering data */

///< Set up all headers of csv files!
void DiaWorld::SetCSVHeaders() {
  std::cerr << "SETTING CSV HEADER" << std::endl;

  // Initialize data trackers
  trt_avg_err.open(config.OUTPUT_DIR() + "trt_avg_err.csv", std::ios::out | std::ios::app);
  trt_min_err.open(config.OUTPUT_DIR() + "trt_min_err.csv", std::ios::out | std::ios::app);
  trt_sol_cnt.open(config.OUTPUT_DIR() + "trt_sol_cnt.csv", std::ios::out | std::ios::app);
  pop_stats.open(config.OUTPUT_DIR() + "pop_stats.csv", std::ios::out | std::ios::app);

  // Create header
  std::string head = "Update,";
  for(size_t i = 0; i < config.K_TRAITS(); i++) {
    std::string cur = (i < config.K_TRAITS()-1) ? "val-" + std::to_string(i) + "," : "val-" + std::to_string(i);
    head += cur;
  }

  trt_avg_err << head << std::endl;
  trt_min_err << head << std::endl;
  trt_sol_cnt << head << std::endl;
  pop_stats << "Gen,Avg_Error,Unique_Genome,Min_Error" << std::endl;
}

///< Will call all other functions to gather data
void DiaWorld::GatherData(size_t up) {
  AverageTraitError(up);
  MinimumTraitError(up);
  CountTraitSolution(up);
  PopulationData(up);
}

///< Given some threshold, how many solutions do we have?
void DiaWorld::CountTraitSolution(size_t up) {
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

///< Average error on a trait, per population, per update
void DiaWorld::AverageTraitError(size_t up) {
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

///< Get best error per trait
void DiaWorld::MinimumTraitError(size_t up) {
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

///< Call functions that return single value pop data
void DiaWorld::PopulationData(size_t up) {
  pop_stats << up << "," << AveragePopError() << "," << UniqueGenomes() << "," << MinPopError()  << std::endl;
}

///< Number of Unique genomes per update
size_t DiaWorld::UniqueGenomes() {
  // This will hold our unique genomes
  std::vector<tar_t> unique_g;

  // Loop through the pop and get genome
  for(size_t i = 0; i < pop.size(); i++) {
    DiaOrg & org = *pop[i];
    tar_t g(org.GetGenome());
    size_t cnt = 0;

    // If this is our first genome
    if(unique_g.size() == 0) {
      unique_g.push_back(g);
      continue;
    }

    // Loop through our existing unique genomes
    for(auto & v : unique_g) {
      // Loop through the unique genome and see if there is a match
      for(size_t j = 0; j < v.size(); j++) {
        // If there is a mismatch at certain positions
        if(v[j] != g[j]) {
          cnt++;
          break;
        }
      }
    }
    if(cnt == unique_g.size()) {
      unique_g.push_back(g);
    }
  }

  return unique_g.size();
}

///< Average error for a population per update
double DiaWorld::AveragePopError() {
  // Hold population error
  double error = 0.0;

  // Iterate through the pop
  for(size_t i = 0; i < pop.size(); i++) {
    DiaOrg & org = *pop[i];
    error += org.GetTotalScore();
  }

  return (error / (double) pop.size());
}

///< Total error for the best performing org in a generation
double DiaWorld::MinPopError() {
  // Max possible error
  double error = config.TARGET() * (double) config.K_TRAITS();
  // tracking the best org
  size_t best = 0;

  for(size_t i = 0; i < pop.size(); i++) {
    DiaOrg & org = *pop[i];

    if(org.GetTotalScore() < error) {
      error = org.GetTotalScore();
      best = i;
    }
  }

  DiaOrg & org = *pop[best];
  return org.GetTotalScore();
}

void DiaWorld::SnapshotPhylogony() {
  sys_ptr->Snapshot(config.OUTPUT_DIR() + "phylo_" + emp::to_string(GetUpdate()) + ".csv");

}

/* DEBUGGINGGGGGGGGGG */

///< Will print all of the cohorts
void DiaWorld::PrintCohorts() {
  // Print everything for population!
  std::cerr << "POPULATION COHORTS:" << std::endl;
  for(size_t c = 0; c < coh_pop_num; c++) {
    std::cerr << "COH-" << c << ": ";
    for(size_t i = 0; i < coh_pop_sze; i++) {
      std::cerr << pop_coh[c][i] << ", ";
    }
    std::cerr << std::endl;
  }
  std::cerr << std::endl;

  // Print everything for traits!
  std::cerr << "TRAIT COHORTS:" << std::endl;
  for(size_t c = 0; c < coh_trt_num; c++) {
    std::cerr << "COH-" << c << ": ";
    for(size_t i = 0; i < coh_trt_sze; i++) {
      std::cerr << trt_coh[c][i] << ", ";
    }
    std::cerr << std::endl;
  }
}

#endif