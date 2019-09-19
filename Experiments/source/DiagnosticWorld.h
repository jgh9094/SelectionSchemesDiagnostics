/// This is the world for DiaOrgs

#ifndef DIA_WORLD_H
#define DIA_WORLD_H

#include "Evolve/World.h"
#include "geometry/Surface.h"
#include "hardware/signalgp_utils.h"
#include "tools/math.h"

#include "config.h"
#include "DiagnosticOrg.h"

class DiaWorld : public emp::World<DiaOrg> {

  private:

    DiaWorldConfig & config;

  public:  
    DiaWorld(DiaWorldConfig & _config) : config(_config)
    {}
};

#endif
