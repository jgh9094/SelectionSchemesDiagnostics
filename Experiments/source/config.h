#ifndef DIA_WORLD_CONFIG_H
#define DIA_WORLD_CONFIG_H

#include "config/config.h"

EMP_BUILD_CONFIG( DiaWorldConfig,
  GROUP(WORLD, "How should the world be set up?"),
  VALUE(POP_SIZE,         size_t,       1,         "How many organisms should we start with?"),  
  VALUE(MAX_GENS,         size_t,       1,          "How many generations should the runs go for?"),
  VALUE(SEED,             int,          0,          "Random number seed (0 for based on time)"),
		  
  GROUP(ENVIRONMENT, "How should the enviorment be set up?"),
  VALUE(TARGET,             double,     100.0,   "A predator cannot consume anything propotionately smaller than this."),
  VALUE(MULTIOBJECTIVE,     bool,       false,   "Will organsims be scored on multiple objectives"), 
  VALUE(SOLUTION_THRESH,    double,     3.0,     "Threshold to determine if a internal value is a solution"),
  VALUE(K_INTERNAL,         size_t,     10,      "How many internal values should each org have?"),
  VALUE(SELECTION,          size_t,     0,       "Which selection are we doing? \n0: Tournament\n1: Lexicase\n2: Cohort Lexicase\n3: Down Sampled Lexicase"),
  VALUE(DIAGNOSTIC,         size_t,     0,       "Which diagnostic are we doing? \n0: Exploitation\n1: Structured Exploitation\n2: Ecology Diagnostic - Contradictory K Values" 
                                                 "\n3: Ecology Diagnostic\n4: Specialist\n5: Hints\n6: Bias\n7: Deceptive\n8: Overfitting - Noise\n9: Exploration"),

  GROUP(TOURNAMENT, "What are the configurations for tournament selection."),
  VALUE(TOUR_SIZE,      size_t,     2,      "How many organimsms are in a tournament?"),
  
  GROUP(MUTATIONS, "Various mutation rates for organisms"),
  VALUE(MUTATE,           bool,       true,         "Do we want mutations in this experiment."),
  VALUE(MUTATE_VAL,       double,     0.001,        "Probability of instructions being mutated"),
  VALUE(MEAN,             double,     0.0,          "Mean of Gaussian Distribution for mutations"),
  VALUE(STD,              double,     2.0,          "Standard Deviation of Gaussian Distribution for mutations"),

  GROUP(OUTPUT, "Output rates for OpenWorld"),
  VALUE(PRINT_INTERVAL,   size_t,     100,          "How many updates between prints?")
)

#endif
