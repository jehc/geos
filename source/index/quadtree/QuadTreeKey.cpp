#include "../../headers/indexQuadtree.h"

namespace geos {

int QuadTreeKey::computeQuadLevel(Envelope *env){
	double dx=env->getWidth();
	double dy=env->getHeight();
	double dMax=dx>dy?dx:dy;
	int level=DoubleBits::exponent(dMax)+1;
	return level;
}

QuadTreeKey::QuadTreeKey(Envelope *itemEnv){
	pt=new Coordinate();
	level=0;
	env=NULL;
	computeKey(itemEnv);
}

QuadTreeKey::~QuadTreeKey(){
	delete pt;
}

Coordinate* QuadTreeKey::getPoint() {
	return pt;
}

int QuadTreeKey::getLevel() {
	return level;
}

Envelope* QuadTreeKey::getEnvelope() {
	return env;
}

Coordinate* QuadTreeKey::getCentre() {
	return new Coordinate(
					(env->getMinX()+env->getMaxX())/2,
					(env->getMinY()+env->getMaxY())/2);
}

/**
* return a square envelope containing the argument envelope,
* whose extent is a power of two and which is based at a power of 2
*/
void QuadTreeKey::computeKey(Envelope *itemEnv) {
	level=computeQuadLevel(itemEnv);
	env=new Envelope();
	computeKey(level,itemEnv);
	// MD - would be nice to have a non-iterative form of this algorithm
	while (!env->contains(itemEnv)) {
		level+=1;
		computeKey(level,itemEnv);
	}
}

void QuadTreeKey::computeKey(int level,Envelope *itemEnv){
	double quadSize=DoubleBits::powerOf2(level);
	//double quadSize=pow2.power(level);
	pt->x=floor(itemEnv->getMinX()/quadSize)*quadSize;
	pt->y=floor(itemEnv->getMinY()/quadSize)*quadSize;
	env->init(pt->x,pt->x+quadSize,pt->y,pt->y+quadSize);
}
}