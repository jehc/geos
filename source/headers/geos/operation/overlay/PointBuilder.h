/**********************************************************************
 * $Id$
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.refractions.net
 *
 * Copyright (C) 2006 Refractions Research Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation. 
 * See the COPYING file for more information.
 *
 **********************************************************************/

#ifndef GEOS_OP_OVERLAY_POINTBUILDER_H
#define GEOS_OP_OVERLAY_POINTBUILDER_H

#include <vector>

#include <geos/geom/GeometryFactory.h> // for inlines

// Forward declarations
namespace geos {
	namespace geom {
		class GeometryFactory;
		class Point;
	}
	namespace geomgraph {
		class Node;
	}
	namespace algorithm {
		class PointLocator;
	}
	namespace operation {
		namespace overlay {
			class OverlayOp;
		}
	}
}

namespace geos {
namespace operation { // geos::operation
namespace overlay { // geos::operation::overlay

/** \brief
 * Constructs geom::Point s from the nodes of an overlay graph.
 */
class PointBuilder {
private:

	OverlayOp *op;
	const geom::GeometryFactory *geometryFactory;
	void extractNonCoveredResultNodes(int opCode);

	/*
	 * Converts non-covered nodes to Point objects and adds them to
	 * the result.
	 *
	 * A node is covered if it is contained in another element Geometry
	 * with higher dimension (e.g. a node point might be contained in
	 * a polygon, in which case the point can be eliminated from
	 * the result).
	 *
	 * @param n the node to test
	 */
	void filterCoveredNodeToPoint(const geomgraph::Node *);

	/// Allocated a construction time, but not owned.
	/// Make sure you take ownership of it, getting 
	/// it from build()
	std::vector<geom::Point*> *resultPointList;

public:

	PointBuilder(OverlayOp *newOp,
			const geom::GeometryFactory *newGeometryFactory,
			algorithm::PointLocator *newPtLocator=NULL)
		:
		op(newOp),
		geometryFactory(newGeometryFactory),
		resultPointList(new std::vector<geom::Point *>())
	{}

	/*
	 * @return a list of the Points in the result of the specified
	 * overlay operation
	 */
	std::vector<geom::Point*>* build(int opCode);
};


} // namespace geos::operation::overlay
} // namespace geos::operation
} // namespace geos

#endif // ndef GEOS_OP_OVERLAY_POINTBUILDER_H

/**********************************************************************
 * $Log$
 * Revision 1.1  2006/03/17 13:24:59  strk
 * opOverlay.h header splitted. Reduced header inclusions in operation/overlay implementation files. ElevationMatrixFilter code moved from own file to ElevationMatrix.cpp (ideally a class-private).
 *
 **********************************************************************/
