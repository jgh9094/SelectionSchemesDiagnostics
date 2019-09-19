///< These are Diagnostic World Organisms.
#ifndef DIA_ORG_H
#define DIA_ORG_H

///< Empirical Headers
#include "geometry/Point2D.h"
#include "hardware/EventDrivenGP.h"

///< Standard Headers


///< Custom Type Names
using genom_t = emp::vector<double>; 

class DiaOrg {

private:
  

public:
  DiaOrg(size_t K) {
  }

  /* Constructor, Destructors, and Copy Constructors */
  DiaOrg(const DiaOrg &) = default;
  DiaOrg(DiaOrg &&) = default;
  ~DiaOrg() { ; }
  DiaOrg & operator=(const DiaOrg &) = default;
  DiaOrg & operator=(DiaOrg &&) = default;

};

#endif
