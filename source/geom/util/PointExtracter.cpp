/**********************************************************************
 * $Id$
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.refractions.net
 *
 * Copyright (C) 2001-2002 Vivid Solutions Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation. 
 * See the COPYING file for more information.
 *
 **********************************************************************
 * $Log$
 * Revision 1.3  2004/05/14 09:20:47  strk
 * Mem leaks fixed
 *
 * Revision 1.2  2004/05/03 20:49:20  strk
 * Some more leaks fixed
 *
 * Revision 1.1  2004/04/04 06:29:11  ybychkov
 * "planargraph" and "geom/utill" upgraded to JTS 1.4
 *
 *
 **********************************************************************/


#include "../../headers/geomUtil.h"
#include <typeinfo>

namespace geos {
/**
* Returns the Point components from a single geometry.
* If more than one geometry is to be processed, it is more
* efficient to create a single {@link PointExtracterFilter} instance
* and pass it to multiple geometries.
*/
vector<Geometry*>* PointExtracter::getPoints(Geometry *geom){
	vector<Geometry*> *ret=new vector<Geometry*>();
	PointExtracter pe(ret);
	geom->apply_rw(&pe);
	return ret;
}

/**
* Constructs a PointExtracterFilter with a list in which to store Points found.
*/
PointExtracter::PointExtracter(vector<Geometry*> *newComps){
	comps=newComps;
}


void PointExtracter::filter_rw(Geometry *geom) {
	if (typeid(*geom)==typeid(Point)) comps->push_back(geom);
}

void PointExtracter::filter_ro(const Geometry *geom) {
	if (typeid(*geom)==typeid(Point)) comps->push_back((Geometry*)geom);
}
}

