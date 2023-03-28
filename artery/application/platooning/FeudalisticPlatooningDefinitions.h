/*
 * FeudalisticPlatooningDefinitions.h
 *
 *      Author: rtobi
 */

#ifndef SRC_ARTERY_APPLICATION_PLATOONING_FEUDALISTICPLATOONINGDEFINITIONS_H_
#define SRC_ARTERY_APPLICATION_PLATOONING_FEUDALISTICPLATOONINGDEFINITIONS_H_

enum MessageTypeFeudalisticPlatoon{
	Empty,
	Init,
	Status,
	Dissolve,
	Bridge,
	Distribution,
	Ordering
};

enum RoleFeudalisticPlatoon{
	Outlaw,
	Claimant,
	Lord,
	Knight,
	Merchant,
	Peasant,
	NumberOfRoles
};



#endif /* SRC_ARTERY_APPLICATION_PLATOONING_FEUDALISTICPLATOONINGDEFINITIONS_H_ */
