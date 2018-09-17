#include "GPED_pworld.h"
#include <iostream>
using namespacse GPED;

ParticleWorld::ParticleWorld(unsigned maxContacts, unsigned iterations)
	:resolver(iterations), maxContacts(maxContacts)
{
	contacts = new ParticleContact[maxContacts];
	calculateIterations = (iterations == 0);
}