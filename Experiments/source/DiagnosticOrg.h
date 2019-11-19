///< These are Diagnostic World Organisms.
#ifndef DIA_ORG_H
#define DIA_ORG_H

///< Empirical Headers
#include "geometry/Point2D.h"
#include "hardware/EventDrivenGP.h"
#include "base/vector.h"

///< Standard Headers
#include <cmath>
#include <limits.h>

///< Custom Type Names
using genome_t = emp::vector<double>;

class DiaOrg {

  private:
    genome_t genome;          ///< Contains the K internal values
    genome_t score;           ///< Contains the scores per K internal values
    genome_t strut_exploit;   ///< Contains the scores for structured exploit
    double best_score;        ///< Contains the best score
    size_t best_index;        ///< Contains the index for the best score
    size_t K;                 ///< How many internal values do you have
    double total;             ///< Contains the total error

  public:

    /* Constructor, Destructors, and Copy Constructors */
    DiaOrg(size_t K_) : best_index(UINT_MAX), best_score(100000), K(K_), total(-1.0) {
      genome.resize(K, 0.0);
      score.resize(K, -1.0);
    }

    DiaOrg(const DiaOrg &) = default;
    DiaOrg(const genome_t g_) : best_index(UINT_MAX), best_score(100000), total(-1.0) { genome = g_; }
    DiaOrg(DiaOrg &&) = default;
    ~DiaOrg() { ; }
    DiaOrg & operator=(const DiaOrg &) = default;
    DiaOrg & operator=(DiaOrg &&) = default;


    /* Getters */
    const genome_t & GetConstGenome() const {return genome;}
    genome_t & GetGenome() { return genome; }
    double GetTotalScore() const { emp_assert(total != -1.0); return total; }
    double GetScore(const size_t & i) const {emp_assert(i >= 0, i); emp_assert(i < score.size(), i); emp_assert(score[i] != -1.0, score[i]); return score[i];}
    double GetTrait(const size_t & i) const {emp_assert(i >= 0, i); emp_assert(i < genome.size(), i); return genome[i];}



    /* Setters */
    void SetGenome(genome_t g) { genome = g; }
    void SetTrait(const size_t & i, const double & _t) {emp_assert(i >= 0, i); emp_assert(i < genome.size(), i); genome[i] = _t;}
    void SetScore(const size_t & i, const double & _s) {emp_assert(i >= 0, i); emp_assert(i < score.size(), i); score[i] = _s;}
    void Reset(size_t K_) {
      K = K_;
      score.resize(K, -1.0);
      total = -1.0;
      best_index = UINT_MAX;
      best_score = 100000;
    }


    /* Calulating scores */

    void DistanceError(size_t & i, const double & target);         ///< Calculate the distance to target
    double TotalSumScores();                                 ///< Sum the total error

    /* Diagnostic specific scoring */

    void ExploitError(size_t & i, double & target);             ///< Will calculate the Exploitation Error
    void StructExploitError(const genome_t & target);           ///< Will calculate the Structured Exploitation Error


    /* DEBUGGING */

    void PrintStats();
};

/* Calulating scores */

void DiaOrg::DistanceError(size_t & i, const double & target) {
  // Make sure we are accessing a valid index
  emp_assert(i >= 0, i);
  emp_assert(i < K, i);

  // Calculate the score internally
  double diff = std::abs(target - genome[i]);
  score[i] = diff;

  // Keep track of the best score and index
  if(diff < best_score) {
    best_score = diff;
    best_index = i;
  }
}

double DiaOrg::TotalSumScores() {
  // Make sure they are the same size and total is new!
  emp_assert(score.size() == K, score.size());
  emp_assert(total == -1.0, total);

  total = 0.0;

  // Sum up the errors!
  for(size_t i = 0; i < K; ++i) {
    // Make sure we have evaluated this internal value
    emp_assert(score[i] != -1.0, score[i]);

    // Sum up
    total += score[i];
  }

  return total;
}


/* Diagnostic specific scoring */

void DiaOrg::ExploitError(size_t & i, double & target) {  ///< Will calculate the Exploitation Error
  DistanceError(i, target);
}

void DiaOrg::StructExploitError(const genome_t & target) { ///< Will calculate the Structured Exploitation Error
  emp_assert(target.size() > 0, target.size());
  emp_assert(target.size() == K, target.size());
  emp_assert(score.size() == K, score.size());

  // Find the fitness for the first trait
  size_t i = 0; bool fail = false;
  DistanceError(i, target[i]);
  i++;

  // Compare the current trait score to the previous to see if we fail
  for(i; i < target.size(); i++) {
    // If we have a better previous score, our condition is broken
    if(GetScore(i-1) <= std::abs(GetTrait(i) - target[i])){
      DistanceError(i, target[i]);
    }
    else {
      fail = true;
      DistanceError(i, target[i]);
      break;
    }
  }

  // If our condition is not met, we can set every value after to the max error
  // For the i value we broke out of the for loop at
  if(fail){
    for(i; i < target.size(); i++){
      DistanceError(i, 0.0);
    }
  }

}


/* DEBUGGING */

void DiaOrg::PrintStats() {
  // Print out all the orgs stuff
  std::cerr << "Genome={";
  for(auto & i : genome) { std::cerr << i << ",";}
  std::cerr << "}" << std::endl;

  std::cerr << "Score={";
  for(auto & i : score) { std::cerr << i << ",";}
  std::cerr << "}" << std::endl;

  std::cerr << "Total=" << total << std::endl;
  std::cerr << "best_score=" << best_score << std::endl;
  std::cerr << "best_index=" << best_index << std::endl;
}

#endif