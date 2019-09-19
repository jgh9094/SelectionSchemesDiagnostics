/// This is the world for DiaOrgs

///< Defnining name
#ifndef DIA_WORLD_H
#define DIA_WORLD_H

///< Standard headers
#include <functional>

///< Empirical headers
#include "Evolve/World.h"
#include "base/vector.h"

///< Experiment headers
#include "config.h"
#include "DiagnosticOrg.h"

///< Custom naming
using fit_fun = std::function<double(DiaOrg &)>; 
using ids_t = emp::vector<size_t>;

class DiaWorld : public emp::World<DiaOrg> {

  private:
    DiaWorldConfig & config;      ///< Holds all the experiments configurations
    fit_fun 


  public:
    DiaWorld(DiaWorldConfig & _config) : config(_config) {      ///< Constructor  
      InitialSetup();
    }

    /* Functions for initial experiment set up */
    
    void InitialSetup();           ///< Do all the initial set up
    void SetFitnessFun();          ///< Set up the fitness function
    void SetMutations();           ///< Set up the mutations parameters
    void SetSelectionFun();        ///< Set up the selection function

    /* Functions ran during experiment */

    ids_t Selection();              ///< Call when its time to select parents
    size_t Mutate();                ///< Call when we need to mutate an organism
    void   Births();                ///< Call when its time to produce offspring


    /* Functions for gathering data */
};

/* Functions for initial experiment set up */
    
void DiaWorld::InitialSetup() {           ///< Do all the initial set up
  SetFitnessFun();
  SetMutations();
  SetSelectionFun();
}
///< Set up the fitness function
void DiaWorld::SetFitnessFun() {    
  // Set up the cases to set the fitness fuction
  switch (config.DIAGNOSTIC()) {
    case 0:  // Exploitation
      /* code */
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
///< Set up the mutations parameters
void DiaWorld::SetMutations() {     

}
///< Set up the selection function
void DiaWorld::SetSelectionFun() {  

}

#endif