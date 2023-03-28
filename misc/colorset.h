/*
 * colorset.h
 *
 *      Author: rtobi
 */

#ifndef SRC_MISC_COLORSET_H_
#define SRC_MISC_COLORSET_H_

#include <vector>
#include "libsumo/TraCIDefs.h"

class Colorsets
{
private:
	std::vector< std::vector<libsumo::TraCIColor> > cs;

public:
	Colorsets(){
		cs.resize(2); // number of sets

		/*** COLOR SET 0 ***/
		cs.at(0).resize(6); // number of colors for first set

		// Pale Turquoise #AFEEEE
		libsumo::TraCIColor tmp;
		tmp.r = 179;
		tmp.g = 238;
		tmp.b = 238;
		tmp.a = 255;
		cs.at(0).at(0) = tmp;

		// Cornflower Blue #6495ED
		tmp.r = 100;
		tmp.g = 149;
		tmp.b = 237;
		tmp.a = 255;
		cs.at(0).at(1) = tmp;

		// Crimson #DC143C
		tmp.r = 220;
		tmp.g = 20;
		tmp.b = 60;
		tmp.a = 255;
		cs.at(0).at(2) = tmp;

		// Khaki #F0E68C
		tmp.r = 240;
		tmp.g = 230;
		tmp.b = 140;
		tmp.a = 255;
		cs.at(0).at(3) = tmp;

		// Orchid #DA70D6
		tmp.r = 218;
		tmp.g = 112;
		tmp.b = 214;
		tmp.a = 255;
		cs.at(0).at(4) = tmp;

		// ForestGreen #228B22
		tmp.r = 34;
		tmp.g = 139;
		tmp.b = 34;
		tmp.a = 255;
		cs.at(0).at(5) = tmp;



		/*** COLOR SET 1 ***/
		cs.at(1).resize(6); // number of colors for second set

		// Pale Turquoise #AFEEEE
		tmp.r = 179;
		tmp.g = 238;
		tmp.b = 238;
		tmp.a = 255;
		cs.at(1).at(0) = tmp;

		// Cornflower Blue #6495ED
		tmp.r = 100;
		tmp.g = 149;
		tmp.b = 237;
		tmp.a = 255;
		cs.at(1).at(1) = tmp;

		// Red - Lord
		tmp.r = 255;
		tmp.g = 0;
		tmp.b = 0;
		tmp.a = 255;
		cs.at(1).at(2) = tmp;

		// Green - Knight
		tmp.r = 0;
		tmp.g = 176;
		tmp.b = 80;
		tmp.a = 255;
		cs.at(1).at(3) = tmp;

		// Purple - Merchant
		tmp.r = 140;
		tmp.g = 64;
		tmp.b = 198;
		tmp.a = 255;
		cs.at(1).at(4) = tmp;

		// No Fill - Peasant
		tmp.r = 0;
		tmp.g = 0;
		tmp.b = 0;
		tmp.a = 255;
		cs.at(1).at(5) = tmp;
	}

	~Colorsets(){};

	std::vector<libsumo::TraCIColor> getColorSet(unsigned int n){
		if(n > cs.size()-1){
			return cs.at(0);
		} else{
			return cs.at(n);
		}
	}
};


#endif /* SRC_MISC_COLORSET_H_ */
