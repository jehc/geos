#include "../../headers/graphindex.h"

namespace geos {

SimpleEdgeSetIntersector::SimpleEdgeSetIntersector() {
	nOverlaps=0;
}

void SimpleEdgeSetIntersector::computeIntersections(vector<Edge*> *edges,SegmentIntersector *si,bool testAllSegments){
	nOverlaps = 0;
	for(int i0=0;i0<(int)edges->size();i0++) {
		Edge *edge0=(*edges)[i0];
		for(int i1=0;i1<(int)edges->size();i1++) {
			Edge *edge1=(*edges)[i1];
			if (testAllSegments || edge0!=edge1)
				computeIntersects(edge0,edge1,si);
		}
	}
}


void SimpleEdgeSetIntersector::computeIntersections(vector<Edge*> *edges0,vector<Edge*> *edges1,SegmentIntersector *si){
	nOverlaps = 0;
	for(int i0=0;i0<(int)edges0->size();i0++) {
		Edge *edge0=(*edges0)[i0];
		for(int i1=0;i1<(int)edges1->size();i1++) {
			Edge *edge1=(*edges1)[i1];
			computeIntersects(edge0,edge1,si);
		}
	}
}

/**
* Performs a brute-force comparison of every segment in each Edge.
* This has n^2 performance, and is about 100 times slower than using
* monotone chains.
*/
void SimpleEdgeSetIntersector::computeIntersects(Edge *e0,Edge *e1,SegmentIntersector *si){
	CoordinateList *pts0=e0->getCoordinates();
	CoordinateList *pts1=e1->getCoordinates();
	for(int i0=0;i0<pts0->getSize()-1;i0++) {
		for(int i1=0;i1<pts1->getSize()-1;i1++) {
			si->addIntersections(e0,i0,e1,i1);
		}
	}
}
}