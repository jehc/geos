/**********************************************************************
 * $Id$
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.refractions.net
 *
 * Copyright (C) 2006      Refractions Research Inc.
 * Copyright (C) 2001-2002 Vivid Solutions Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Licence as published
 * by the Free Software Foundation. 
 * See the COPYING file for more information.
 *
 **********************************************************************
 *
 * Last port: noding/SegmentNodeList.java rev. 1.7 (JTS-1.7)
 *
 **********************************************************************/

#include <cassert>
#include <geos/noding.h>
#include <geos/profiler.h>

namespace geos {

#if PROFILE
static Profiler *profiler = Profiler::instance();
#endif


SegmentNodeList::~SegmentNodeList()
{
	set<SegmentNode *, SegmentNodeLT>::iterator it=nodeMap.begin();
	for(; it!=nodeMap.end(); it++) delete *it;

	unsigned int i=0;

	for(; i<splitEdges.size(); i++) delete splitEdges[i];

	unsigned int n=splitCoordLists.size();
	for(i=0; i<n; i++) delete splitCoordLists[i];
}

SegmentNode*
SegmentNodeList::add(const Coordinate& intPt, unsigned int segmentIndex)
{
	SegmentNode *eiNew=new SegmentNode(edge, intPt, segmentIndex,
			edge.getSegmentOctant(segmentIndex));

	pair<SegmentNodeList::iterator,bool> p = nodeMap.insert(eiNew);
	if ( p.second ) { // new SegmentNode inserted
		return eiNew;
	} else {

		// sanity check 
		assert(eiNew->coord.equals2D(intPt));

		delete eiNew;
		return *(p.first);
	}
}

void SegmentNodeList::addEndpoints()
{
	int maxSegIndex = edge.size() - 1;
	add(&(edge.getCoordinate(0)), 0);
	add(&(edge.getCoordinate(maxSegIndex)), maxSegIndex);
}

/* private */
void
SegmentNodeList::addCollapsedNodes()
{
	vector<unsigned int> collapsedVertexIndexes;

	findCollapsesFromInsertedNodes(collapsedVertexIndexes);
	findCollapsesFromExistingVertices(collapsedVertexIndexes);

	// node the collapses
	for (vector<unsigned int>::iterator
		i=collapsedVertexIndexes.begin(),
			e=collapsedVertexIndexes.end();
		i != e; ++i)
	{
		unsigned int vertexIndex = *i;
		add(edge.getCoordinate(vertexIndex), vertexIndex);
	}
}


/* private */
void
SegmentNodeList::findCollapsesFromExistingVertices(
			vector<unsigned int>& collapsedVertexIndexes)
{
	for (unsigned int i=0, n=edge.size()-2; i<n; ++i)
	{
		const Coordinate& p0 = edge.getCoordinate(i);
		//const Coordinate& p1 = edge.getCoordinate(i + 1);
		const Coordinate& p2 = edge.getCoordinate(i + 2);
		if (p0.equals2D(p2)) {
			// add base of collapse as node
			collapsedVertexIndexes.push_back(i + 1);
		}
	}
}

/* private */
void
SegmentNodeList::findCollapsesFromInsertedNodes(
		vector<unsigned int>& collapsedVertexIndexes)
{
	unsigned int collapsedVertexIndex;

	// there should always be at least two entries in the list,
	// since the endpoints are nodes
	iterator it = begin();
	SegmentNode* eiPrev = *it;
	++it;
	for(iterator itEnd=end(); it!=itEnd; ++it)
	{
		SegmentNode *ei=*it;
      		bool isCollapsed = findCollapseIndex(*eiPrev, *ei,
				collapsedVertexIndex);
		if (isCollapsed)
			collapsedVertexIndexes.push_back(collapsedVertexIndex);

		eiPrev = ei;
	}
}

/* private */
bool
SegmentNodeList::findCollapseIndex(SegmentNode& ei0, SegmentNode& ei1,
		unsigned int& collapsedVertexIndex)
{
	// only looking for equal nodes
	if (! ei0.coord.equals2D(ei1.coord)) return false;

	int numVerticesBetween = ei1.segmentIndex - ei0.segmentIndex;
	if (! ei1.isInterior()) {
		numVerticesBetween--;
	}

	// if there is a single vertex between the two equal nodes,
	// this is a collapse
	if (numVerticesBetween == 1) {
		collapsedVertexIndex = ei0.segmentIndex + 1;
		return true;
	}
	return false;
}


/* public */
void
SegmentNodeList::addSplitEdges(vector<SegmentString*>& edgeList)
{
	// testingOnly
	//vector<SegmentString*> testingSplitEdges;

	// ensure that the list has entries for the first and last
	// point of the edge
	addEndpoints();
	addCollapsedNodes();

	// there should always be at least two entries in the list
	// since the endpoints are nodes
	iterator it=begin();
	SegmentNode *eiPrev=*it;
	it++;
	for(iterator itEnd=end(); it!=itEnd; ++it)
	{
		SegmentNode *ei=*it;
		SegmentString *newEdge=createSplitEdge(eiPrev, ei);
		edgeList.push_back(newEdge);
		//testingSplitEdges.push_back(newEdge);
		eiPrev = ei;
	}
	//checkSplitEdgesCorrectness(testingSplitEdges);
}

void
SegmentNodeList::checkSplitEdgesCorrectness(vector<SegmentString*>& splitEdges)
{
	const CoordinateSequence *edgePts=edge.getCoordinatesRO();

	// check that first and last points of split edges
	// are same as endpoints of edge
	SegmentString *split0=splitEdges[0];
	const Coordinate& pt0=split0->getCoordinate(0);
	if (!(pt0==edgePts->getAt(0)))
		throw GEOSException("bad split edge start point at " + pt0.toString());

	SegmentString *splitn=splitEdges[splitEdges.size()-1];
	const CoordinateSequence *splitnPts=splitn->getCoordinatesRO();
	const Coordinate &ptn=splitnPts->getAt(splitnPts->getSize()-1);
	if (!(ptn==edgePts->getAt(edgePts->getSize()-1)))
		throw  GEOSException("bad split edge end point at " + ptn.toString());
}

/**
 * Create a new "split edge" with the section of points between
 * (and including) the two intersections.
 * The label for the new edge is the same as the label for the parent edge.
 */
SegmentString*
SegmentNodeList::createSplitEdge(SegmentNode *ei0, SegmentNode *ei1)
{
	int npts = ei1->segmentIndex - ei0->segmentIndex + 2;
	const Coordinate &lastSegStartPt=edge.getCoordinate(ei1->segmentIndex);

	// if the last intersection point is not equal to the its
	// segment start pt, add it to the points list as well.
	// (This check is needed because the distance metric is not
	// totally reliable!)

	// The check for point equality is 2D only - Z values are ignored
	bool useIntPt1 = ei1->isInterior() || \
			! ei1->coord.equals2D(lastSegStartPt);

	if (! useIntPt1) {
		npts--;
	}

	CoordinateSequence *pts = new CoordinateArraySequence(npts); 
	unsigned int ipt = 0;
	pts->setAt(ei0->coord, ipt++);
	for (unsigned int i=ei0->segmentIndex+1; i<=ei1->segmentIndex; i++)
	{
		pts->setAt(edge.getCoordinate(i),ipt++);
	}
	if (useIntPt1) 	pts->setAt(ei1->coord, ipt++);

	SegmentString *ret = new SegmentString(pts,edge.getContext());
	splitEdges.push_back(ret);

	// Keep track of created CoordinateSequence to release
	// it at this SegmentNodeList destruction time
	splitCoordLists.push_back(pts);

	return ret;
}

string SegmentNodeList::print(){
	string out="Intersections:";
	set<SegmentNode*,SegmentNodeLT>::iterator it=nodeMap.begin();
	for(;it!=nodeMap.end();it++) {
		SegmentNode *ei=*it;
		out.append(ei->print());
	}
	return out;
}

} // namespace geos

/**********************************************************************
 * $Log$
 * Revision 1.20  2006/02/15 14:59:07  strk
 * JTS-1.7 sync for:
 * noding/SegmentNode.cpp
 * noding/SegmentNodeList.cpp
 * noding/SegmentString.cpp
 *
 * Revision 1.19  2006/02/14 13:28:26  strk
 * New SnapRounding code ported from JTS-1.7 (not complete yet).
 * Buffer op optimized by using new snaprounding code.
 * Leaks fixed in XMLTester.
 *
 * Revision 1.18  2006/02/09 15:52:47  strk
 * GEOSException derived from std::exception; always thrown and cought by const ref.
 *
 * Revision 1.17  2006/01/31 19:07:34  strk
 * - Renamed DefaultCoordinateSequence to CoordinateArraySequence.
 * - Moved GetNumGeometries() and GetGeometryN() interfaces
 *   from GeometryCollection to Geometry class.
 * - Added getAt(int pos, Coordinate &to) funtion to CoordinateSequence class.
 * - Reworked automake scripts to produce a static lib for each subdir and
 *   then link all subsystem's libs togheter
 * - Moved C-API in it's own top-level dir capi/
 * - Moved source/bigtest and source/test to tests/bigtest and test/xmltester
 * - Fixed PointLocator handling of LinearRings
 * - Changed CoordinateArrayFilter to reduce memory copies
 * - Changed UniqueCoordinateArrayFilter to reduce memory copies
 * - Added CGAlgorithms::isPointInRing() version working with
 *   Coordinate::ConstVect type (faster!)
 * - Ported JTS-1.7 version of ConvexHull with big attention to
 *   memory usage optimizations.
 * - Improved XMLTester output and user interface
 * - geos::geom::util namespace used for geom/util stuff
 * - Improved memory use in geos::geom::util::PolygonExtractor
 * - New ShortCircuitedGeometryVisitor class
 * - New operation/predicate package
 *
 * Revision 1.16  2005/11/07 18:05:28  strk
 * Reduced set<> lookups
 *
 * Revision 1.15  2005/02/22 18:21:46  strk
 * Changed SegmentNode to contain a *real* Coordinate (not a pointer) to reduce
 * construction costs.
 *
 * Revision 1.14  2005/02/05 05:44:47  strk
 * Changed geomgraph nodeMap to use Coordinate pointers as keys, reduces
 * lots of other Coordinate copies.
 *
 * Revision 1.13  2005/02/01 14:18:36  strk
 * More profiler labels
 *
 * Revision 1.12  2005/02/01 13:44:59  strk
 * More profiling labels.
 *
 * Revision 1.11  2004/11/01 16:43:04  strk
 * Added Profiler code.
 * Temporarly patched a bug in DoubleBits (must check drawbacks).
 * Various cleanups and speedups.
 *
 * Revision 1.10  2004/07/13 08:33:53  strk
 * Added missing virtual destructor to virtual classes.
 * Fixed implicit unsigned int -> int casts
 *
 * Revision 1.9  2004/07/08 19:34:49  strk
 * Mirrored JTS interface of CoordinateSequence, factory and
 * default implementations.
 * Added CoordinateArraySequenceFactory::instance() function.
 *
 * Revision 1.8  2004/07/02 13:28:27  strk
 * Fixed all #include lines to reflect headers layout change.
 * Added client application build tips in README.
 *
 * Revision 1.7  2004/07/01 14:12:44  strk
 *
 * Geometry constructors come now in two flavors:
 * 	- deep-copy args (pass-by-reference)
 * 	- take-ownership of args (pass-by-pointer)
 * Same functionality is available through GeometryFactory,
 * including buildGeometry().
 *
 * Revision 1.6  2004/06/16 13:13:25  strk
 * Changed interface of SegmentString, now copying CoordinateSequence argument.
 * Fixed memory leaks associated with this and MultiGeometry constructors.
 * Other associated fixes.
 *
 * Revision 1.5  2004/05/27 10:27:03  strk
 * Memory leaks fixed.
 *
 * Revision 1.4  2004/05/06 15:54:15  strk
 * SegmentNodeList keeps track of created splitEdges for later destruction.
 * SegmentString constructor copies given Label.
 * Buffer operation does no more leaks for doc/example.cpp
 *
 * Revision 1.3  2004/05/03 22:56:44  strk
 * leaks fixed, exception specification omitted.
 *
 * Revision 1.2  2004/05/03 20:49:20  strk
 * Some more leaks fixed
 *
 * Revision 1.1  2004/03/26 07:48:30  ybychkov
 * "noding" package ported (JTS 1.4)
 *
 *
 **********************************************************************/

