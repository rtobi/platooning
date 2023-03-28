/*
 * PlatooningServiceEnvmod.cc
 *
 *      Author: rtobi
 */
#include "PlatooningServiceEnvmod.h"
#include "artery/envmod/LocalEnvironmentModel.h"
#include "artery/envmod/service/EnvmodPrinter.h"

namespace artery{
namespace platooning{
using namespace omnetpp;

Define_Module(PlatooningServiceEnvmod)

void PlatooningServiceEnvmod::initialize()
{
	PlatooningService::initialize();
    localEnvmod = &getFacilities().get_const<artery::LocalEnvironmentModel>();
}

} //namespace platooning
} //namespace artery
