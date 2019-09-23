///< These are Diagnostic World Organisms.
#ifndef DIA_ORG_H
#define DIA_ORG_H

///< Empirical Headers
#include "geometry/Point2D.h"
#include "hardware/EventDrivenGP.h"
#include "base/vector.h"

///< Standard Headers
#include <cmath>

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
    DiaOrg(size_t K_) : best_index(-1), best_score(100000), K(K_), total(0.0) {
      genome.resize(K, 0.0);
      score.resize(K, -1.0);
    }
    
    DiaOrg(const DiaOrg &) = default;
    DiaOrg(DiaOrg &&) = default;
    ~DiaOrg() { ; }
    DiaOrg & operator=(const DiaOrg &) = default;
    DiaOrg & operator=(DiaOrg &&) = default;

    /* Getters */ 
    const genome_t & GetConstGenome() const { return genome; }
    genome_t & GetGenome() { return genome; }

    /* Setters */
    void SetGenome(genome_t g) { genome = g; }

    /* Calulating scores */

    void CalculateScore(size_t & i, double & truth);        ///< Calculate the score for a internal value
    double TotalSumScores();                                 ///< Calculate the total error
};

void DiaOrg::CalculateScore(size_t & i, double & truth) {
  // Make sure we are accessing a valid index
  emp_assert(i > = 0, i);
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
  emp_assert(total == 0.0, total);

  // Sum up the errors!
  double 
  for(size_t i = 0; i < K; ++i) {
    // Make sure we have evaluated this internal value
    emp_assert(score[i] != -1.0, score[i]);

    // Sum up
    total += score[i];
  }

  return total; 
}

#endif
