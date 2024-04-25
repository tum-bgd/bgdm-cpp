#ifndef DEFAULT_GEOMETRY_INC
#define DEFAULT_GEOMETRY_INC


#include <boost/geometry.hpp>
namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

  typedef bg::model::point<double, 2, bg::cs::cartesian> point_type;
  
  /* Deriving a full OGC Simple Features frameowrk of types */
  typedef bg::model::segment<point_type>  segment_type;
  
  typedef bg::model::multi_point<point_type>  multipoint_type; 
  typedef bg::model::box<point_type> box_type;
  typedef bg::model::polygon<point_type> polygon_type; 
  typedef bg::model::multi_polygon<polygon_type> multipolygon_type; 
  typedef bg::model::linestring<point_type> linestring_type;
  typedef bg::model::multi_linestring<linestring_type> multilinestring_type; 
  typedef std::pair<box_type, unsigned> value_type;
  /* An indexing engine */
  typedef bgi::rtree< value_type, bgi::rstar<16, 4> > rtree_type;	

  /* Type Conversion Tools */
  struct value_maker
  {
    template<typename T>
    inline value_type operator()( T const& v) const
    {
      return value_type(v.value(), v.index());
    }
   };



#endif
