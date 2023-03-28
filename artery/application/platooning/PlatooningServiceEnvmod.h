/*
 * PlatooningServiceEnvmod.h
 *
 *      Author: rtobi
 */
#ifndef PLATOONINGSERVICEENVMOD_H
#define PLATOONINGSERVICEENVMOD_H

#include "PlatooningService.h"

// forward declaration
namespace artery {
class LocalEnvironmentModel;

namespace platooning{

// Extends PlatooningService by using envmod features
class PlatooningServiceEnvmod : public PlatooningService
{
    public:
        void initialize() override;

    private:
        const artery::LocalEnvironmentModel* localEnvmod;
};

} //namespace platooning
} //namespace artery

#endif /* PLATOONINGSERVICEENVMOD_H*/
