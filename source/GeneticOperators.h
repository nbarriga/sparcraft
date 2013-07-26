/*
 * GeneticOperators.h
 *
 *  Created on: Jul 26, 2013
 *      Author: nbarriga
 */

#ifndef GENETICOPERATORS_H_
#define GENETICOPERATORS_H_

#include <ga/GAListGenome.h>

namespace SparCraft {

class GeneticOperators {
public:
	static float Objective(GAGenome &g);
	static void	Initializer(GAGenome& g);
	static int Mutator(GAGenome& g, float pmut);
};

} /* namespace SparCraft */
#endif /* GENETICOPERATORS_H_ */
