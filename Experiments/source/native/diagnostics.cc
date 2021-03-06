// This is the main function for the NATIVE version of this project.

#include <iostream>

#include "base/vector.h"
#include "config/command_line.h"
#include "config/ArgManager.h"

#include "../config.h"
#include "../DiagnosticWorld.h"

int main(int argc, char* argv[])
{
  std::string config_name = "DiaWorld.cfg";
  auto args = emp::cl::ArgManager(argc, argv);
  DiaWorldConfig config;
  config.Read(config_name);

  if (args.ProcessConfigOptions(config, std::cout, config_name, "") == false)
    exit(0);
  if (args.TestUnknown() == false)
    exit(0);

  std::cout << "==============================" << std::endl;
  std::cout << "|    How am I configured?    |" << std::endl;
  std::cout << "==============================" << std::endl;
  config.Write(std::cout);
  std::cout << "==============================\n"
            << std::endl;

  DiaWorld world(config);
  std::cerr << "Beginning Experiment" << std::endl;
  for(size_t i = 0; i < config.MAX_GENS(); i++) {
    std::cerr << "------------------------------------------------" << std::endl;
    std::cerr << "Gen=" << i << std::endl;
    world.Update();
    std::cerr << "------------------------------------------------" << std::endl;
  }

}
