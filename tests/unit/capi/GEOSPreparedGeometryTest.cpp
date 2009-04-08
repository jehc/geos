// $Id$
// 
// Test Suite for C-API GEOSPreparedGeometry

// TUT
#include <tut.h>
// GEOS CAPI
#include <geos_c.h>
// C+
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <memory>

namespace tut
{
    //
    // Test Group
    //

    // Common data used in test cases.
    struct test_capigeospreparedgeometry_data
	{
	    GEOSGeometry* geom1_;
	    GEOSGeometry* geom2_;

	    const GEOSPreparedGeometry* prepGeom1_;
	    const GEOSPreparedGeometry* prepGeom2_;



		static void notice(const char *fmt, ...)
		{
			std::fprintf( stdout, "NOTICE: ");

			va_list ap;
			va_start(ap, fmt);
			std::vfprintf(stdout, fmt, ap);
			va_end(ap);
		
			std::fprintf(stdout, "\n");
		}

		test_capigeospreparedgeometry_data()
            : geom1_(0), geom2_(0), prepGeom1_(0), prepGeom2_(0)
		{
			initGEOS(notice, notice);
		}		

		~test_capigeospreparedgeometry_data()
		{
            GEOSGeom_destroy(geom1_);
            GEOSGeom_destroy(geom2_);
	    GEOSPreparedGeom_destroy(prepGeom1_);
	    GEOSPreparedGeom_destroy(prepGeom2_);
            geom1_ = 0;
            geom2_ = 0;
            prepGeom1_ = 0;
            prepGeom2_ = 0;
			finishGEOS();
		}

	};

	typedef test_group<test_capigeospreparedgeometry_data> group;
	typedef group::object object;

	group test_capigeospreparedgeometry_group("capi::GEOSPreparedGeometry");

    //
    // Test Cases
    //

    // Test geometry preparation
    template<>
    template<>
    void object::test<1>()
    {
	    	geom1_ = GEOSGeomFromWKT("POLYGON EMPTY");
		prepGeom1_ = GEOSPrepare(geom1_);

		// ownership NOT transferred to prepGeom1, despite
		// docs say so
		//geom1_ = 0;

		ensure(prepGeom1_);

    }

    // Test PreparedContainsProperly
    // Taken from regress/regress_ogc_prep.sql of postgis
    // as of revno 3936 
    // ref: containsproperly200 (a)
    template<>
    template<>
    void object::test<2>()
    {
	geom1_ = GEOSGeomFromWKT("POLYGON((0 0, 0 10, 10 10, 10 0, 0 0))");
	geom2_ = GEOSGeomFromWKT("POLYGON((2 2, 2 3, 3 3, 3 2, 2 2))");
	prepGeom1_ = GEOSPrepare(geom1_);

	// ownership NOT transferred to prepGeom1, despite
	// docs say so
	//geom1_ = 0;

	ensure(prepGeom1_);

	int ret = GEOSPreparedContainsProperly(prepGeom1_, geom2_);
	ensure_equals(ret, 1);

    }

    // Test PreparedContainsProperly
    // Taken from regress/regress_ogc_prep.sql of postgis
    // as of revno 3936 
    // ref: containsproperly200 (b)
    template<>
    template<>
    void object::test<3>()
    {
	geom1_ = GEOSGeomFromWKT("POLYGON((2 2, 2 3, 3 3, 3 2, 2 2))");
	geom2_ = GEOSGeomFromWKT("POLYGON((0 0, 0 10, 10 10, 10 0, 0 0))");
	prepGeom1_ = GEOSPrepare(geom1_);

	// ownership NOT transferred to prepGeom1, despite
	// docs say so
	//geom1_ = 0;

	ensure(prepGeom1_);

	int ret = GEOSPreparedContainsProperly(prepGeom1_, geom2_);
	ensure_equals(ret, 0);

    }

    // TODO: add lots of more tests
    
} // namespace tut

