/*
 * GeneticOperators.cpp
 *
 *  Created on: Jul 26, 2013
 *      Author: nbarriga
 */

#include "GeneticOperators.h"
#include "Gene.h"

namespace SparCraft {

float GeneticOperators::Objective(GAGenome &g) {
	return 0;
}

void GeneticOperators::Initializer(GAGenome& g)
{
	GAListGenome<Gene>& genome = (GAListGenome<Gene>&)g;
	while(genome.head()) genome.destroy(); // destroy any pre-existing list

	genome.insert(Gene(),GAListBASE::HEAD);
	genome.insert(Gene(),GAListBASE::HEAD);
	genome.insert(Gene(),GAListBASE::HEAD);
}

int GeneticOperators::Mutator(GAGenome& g, float pmut)
{
	GAListGenome<Gene>& genome = (GAListGenome<Gene>&)g;


	if(pmut <= 0.0) return 0;


	int nMut = 0;

	for(int i=0; i<genome.size(); i++){
		if(GAFlipCoin(pmut)){
			Gene* gene=genome[i];
			std::cout<<"mutating"<<std::endl;
			gene->mutate();
			nMut++;
		}
	}

	return nMut;

}

} /* namespace SparCraft */
