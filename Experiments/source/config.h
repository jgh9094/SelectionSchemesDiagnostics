#ifndef DIA_WORLD_CONFIG_H
#define DIA_WORLD_CONFIG_H

#include "config/config.h"

EMP_BUILD_CONFIG( DiaWorldConfig,
  GROUP(WORLD, "How should the world be set up?"),
  VALUE(POP_SIZE, size_t, 100, "How many organisms should we start with?"),  
  VALUE(MAX_GENS, size_t, 0, "How many generations should the runs go for?"),
  VALUE(SEED, int, 0, "Random number seed (0 for based on time)"),
		  
  GROUP(ENVIRONMENT, "How should the enviorment be set up?"),
  VALUE(TARGET,         double,     100.0,   "A predator cannot consume anything propotionately smaller than this."),
  VALUE(K_INTERNAL,     size_t,     10,      "How many internal values should each org have?"),
  VALUE(DIAGNOSTIC,     size_t,     0,       "Which diagnostic are we doing? \n0: Exploitation\n1: Structured Exploitation\n2: Ecology Diagnostic - Contradictory K Values" 
                                              "\n3: Ecology Diagnostic\n4: Specialist\n5: Hints\n6: Bias\n7: Deceptive\n8: Overfitting - Noise\n9: Exploration"),
  
  GROUP(MUTATIONS, "Various mutation rates for SignalGP Brains"),
  VALUE(MUTATE_VAL, double, 0.001, "Probability of instructions being mutated"),

  GROUP(OUTPUT, "Output rates for OpenWorld"),
  VALUE(PRINT_INTERVAL, size_t, 100, "How many updates between prints?")
)

#endif
