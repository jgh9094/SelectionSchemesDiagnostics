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
    const genome_t & GetConstGenome() const { return genome; }
    genome_t & GetGenome() { return genome; }
    double GetTotal() const { emp_assert(total != -1.0); return total; }

    /* Setters */
    void SetGenome(genome_t g) { genome = g; }
    void Reset(size_t K_); 

    /* Calulating scores */

    void CalculateScore(size_t & i, double & truth);        ///< Calculate the score for a internal value
    double TotalSumScores();                                 ///< Calculate the total error

    /* DEBUGGING */

    void PrintStats(); 
};

void DiaOrg::CalculateScore(size_t & i, double & truth) {
  // Make sure we are accessing a valid index
  emp_assert(i >= 0, i);
  emp_assert(i < K, i);

  // Calculate the score internally 
  double diff = std::abs(truth - genome[i]);
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

void DiaOrg::Reset(size_t K_) { 
  K = K_; 
  score.resize(K, -1.0); 
  total = -1.0; 
  best_index = UINT_MAX;
  best_score = 100000;
}
#endif