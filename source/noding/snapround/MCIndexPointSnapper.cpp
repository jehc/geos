/**********************************************************************
 * $Id$
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.refractions.net
 *
 * Copyright (C) 2006 Refractions Research Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Licence as published
 * by the Free Software Foundation. 
 * See the COPYING file for more information.
 *
 **********************************************************************
 *
 * Last port: noding/snapround/MCIndexPointSnapper.java rev. 1.2 (JTS-1.7)
 *
 **********************************************************************/

#include "geos/nodingSnapround.h"

using namespace geos::index;
using namespace geos::index::chain;

namespace geos {
namespace noding { // geos.noding
namespace snapround { // geos.noding.snapround

class HotPixelSnapAction: public MonotoneChainSelectAction {

private:
	const HotPixel& hotPixel;
	SegmentString* parentEdge;
	unsigned int vertexIndex;
	bool isNodeAddedVar;

public:

	HotPixelSnapAction(const HotPixel& nHotPixel,
			SegmentString* nParentEdge,
			unsigned int nVertexIndex)
		:
		hotPixel(nHotPixel),
		parentEdge(nParentEdge),
		vertexIndex(nVertexIndex),
		isNodeAddedVar(false)
	{}

	bool isNodeAdded() const { return isNodeAddedVar; }

	void select(MonotoneChain& mc, unsigned int startIndex)
	{
		// This is casting away 'constness'!
		SegmentString& ss = *(static_cast<SegmentString*>(mc.getContext()));

		// don't snap a vertex to itself
		if ( parentEdge ) {
			if (&ss == parentEdge && startIndex == vertexIndex) return;
		}
		isNodeAddedVar = SimpleSnapRounder::addSnappedNode(hotPixel, ss, startIndex);
	}

	void select(LineSegment* ls) {}

};

class MCIndexPointSnapperVisitor: public ItemVisitor {
private:
	const Envelope& pixelEnv;
	MonotoneChainSelectAction& action;

public:
	MCIndexPointSnapperVisitor(const Envelope& nPixelEnv, HotPixelSnapAction& nAction)
		:
		pixelEnv(nPixelEnv),
		action(nAction)
	{}

	virtual ~MCIndexPointSnapperVisitor() {}

	void visitItem(void* item) {
		MonotoneChain& testChain = *(static_cast<MonotoneChain*>(item));
		testChain.select(pixelEnv, action);
	}
};

/* public */
bool
MCIndexPointSnapper::snap(const HotPixel& hotPixel,
		SegmentString* parentEdge,
		unsigned int vertexIndex)
{
	const Envelope& pixelEnv = hotPixel.getSafeEnvelope();
	HotPixelSnapAction hotPixelSnapAction(hotPixel, parentEdge, vertexIndex);
	MCIndexPointSnapperVisitor visitor(pixelEnv, hotPixelSnapAction);

	index.query(&pixelEnv, visitor);

	return hotPixelSnapAction.isNodeAdded();
}
 
} // namespace geos.noding.snapround
} // namespace geos.noding
} // namespace geos

/**********************************************************************
 * $Log$
 * Revision 1.5  2006/02/21 16:53:49  strk
 * MCIndexPointSnapper, MCIndexSnapRounder
 *
 * Revision 1.4  2006/02/20 10:14:18  strk
 * - namespaces geos::index::*
 * - Doxygen documentation cleanup
 *
 * Revision 1.3  2006/02/19 19:46:49  strk
 * Packages <-> namespaces mapping for most GEOS internal code (uncomplete, but working). Dir-level libs for index/ subdirs.
 *
 * Revision 1.2  2006/02/18 21:08:09  strk
 * - new CoordinateSequence::applyCoordinateFilter method (slow but useful)
 * - SegmentString::getCoordinates() doesn't return a clone anymore.
 * - SegmentString::getCoordinatesRO() obsoleted.
 * - SegmentString constructor does not promises constness of passed
 *   CoordinateSequence anymore.
 * - NEW ScaledNoder class
 * - Stubs for MCIndexPointSnapper and  MCIndexSnapRounder
 * - Simplified internal interaces of OffsetCurveBuilder and OffsetCurveSetBuilder
 *
 * Revision 1.1  2006/02/14 13:28:26  strk
 * New SnapRounding code ported from JTS-1.7 (not complete yet).
 * Buffer op optimized by using new snaprounding code.
 * Leaks fixed in XMLTester.
 *
 **********************************************************************/