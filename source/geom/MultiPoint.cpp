#include "../headers/geom.h"
#include "../headers/operation.h"

namespace geos {

MultiPoint::MultiPoint(){}
MultiPoint::MultiPoint(vector<Geometry *> *points,PrecisionModel* pm, int SRID):
GeometryCollection(points, pm, SRID){}
MultiPoint::~MultiPoint(){}

int MultiPoint::getDimension() {
	return 0;
}

int MultiPoint::getBoundaryDimension() {
	return Dimension::False;
}

string MultiPoint::getGeometryType() {
	return "MultiPoint";
}

Geometry* MultiPoint::getBoundary() {
	return new GeometryCollection(NULL, precisionModel, SRID);
}

bool MultiPoint::isSimple(){
	return (new IsSimpleOp())->isSimple(this);
}

bool MultiPoint::isValid() {
	return true;
}

bool MultiPoint::equalsExact(Geometry *other, double tolerance) {
    if (!isEquivalentClass(other)) {
      return false;
    }
	return GeometryCollection::equalsExact(other,tolerance);
  }

Coordinate* MultiPoint::getCoordinate(int n) {
	return ((Point *)(*geometries)[n])->getCoordinate();
}
}