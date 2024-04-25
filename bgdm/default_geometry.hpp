#ifndef DEFAULT_GEOMETRY_INC
#define DEFAULT_GEOMETRY_INC


#include <boost/geometry.hpp>
namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

  typedef bg::model::point<double, DIM, bg::cs::cartesian> point_type;
  
  /* Deriving a full OGC Simple Features frameowrk of types */
  typedef bg::model::multi_point<point_type>  multipoint_type; 
  typedef bg::model::box<point_type> box_type;
  typedef bg::model::polygon<point_type> polygon_type; 
  typedef bg::model::multi_polygon<polygon_type> multipolygon_type; 
  typedef bg::model::linestring<point_type> linestring_type;
  typedef std::pair<box_type, unsigned> value_type;
  /*Attributes*/
  typedef std::unordered_map<std::string, std::vector<double>> named_attrib_type;

  /* An indexing engine */
  typedef bgi::rtree< value_type, bgi::rstar<16, 4> > rtree_type;	

  /* Type Conversion Tools */
  struct value_maker
  {
    template<typename T>
    inline value_type operator()(T const& v) const
    {
      box_type b;
      bg::envelope(b,v);//(v.value(), v.value()); // make point to box
        return value_type(b, v.index());
    }
   };

}



#endif
