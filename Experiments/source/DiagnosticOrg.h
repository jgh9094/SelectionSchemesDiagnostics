///< These are Diagnostic World Organisms.
#ifndef DIA_ORG_H
#define DIA_ORG_H

///< Empirical Headers
#include "geometry/Point2D.h"
#include "hardware/EventDrivenGP.h"
#include "base/vector.h"

///< Standard Headers


///< Custom Type Names
using genome_t = emp::vector<double>; 

class DiaOrg {

  private:
    genome_t genome;    ///< Contains the K internal values    

  public:

    /* Constructor, Destructors, and Copy Constructors */
    DiaOrg(size_t K) {
      genome.resize(K, 0.0);
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
};

#endif
