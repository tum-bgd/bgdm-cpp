#include "bgdm-cpp/bgdm/default_geometry2double.h"

#include<random>
#include<iostream>


point_type random_point(){
  static std::random_device rd;
  static std::mt19937 mt(rd());
  static std::uniform_real_distribution<double> dist(-10, 10.0);
  return point_type(dist(mt),dist(mt));
}

box_type random_box()
{
   return box_type(point_type(-1,-1),point_type(1,1));
}




template<typename outputiterator>
void cut_box_out_of_segment(segment_type s, box_type b, outputiterator out){
    //segment_type s{{3,4},{10,4}};
    //box_type b{{4,4},{6,6}};

    int n=0; 
    // this cuts out the box from the segment
    // returns either the segment (in case no intersection)
    if (!bg::intersects(s,b)) {
     *out++=s;
    return;
    }
    // ray casting unordered
    const auto base = point_type(bg::get<0,0>(s),bg::get<0,1>(s)),
           endpoint = point_type(bg::get<1,0>(s),bg::get<1,1>(s));
    std::map<double, point_type> pointset; // distance, point
    
    std::vector<point_type> ips;
    bg::intersection(s,segment_type(
        {bg::get<0,0>(b),bg::get<0,1>(b)},
        {bg::get<0,0>(b),bg::get<1,1>(b)}
	),
	ips);
    for (auto &p: ips)
	pointset.insert({bg::distance(p,base),p});
    bg::intersection(s,segment_type(
        {bg::get<0,0>(b),bg::get<1,1>(b)},
        {bg::get<1,0>(b),bg::get<1,1>(b)}
	),
	ips);
    for (auto &p: ips)
	pointset.insert({bg::distance(p,base),p});
    bg::intersection(s,segment_type(
        {bg::get<1,0>(b),bg::get<1,1>(b)},
        {bg::get<1,0>(b),bg::get<0,1>(b)}
	),
	ips);
    for (auto &p: ips)
	pointset.insert({bg::distance(p,base),p});
    bg::intersection(s,segment_type(
        {bg::get<1,0>(b),bg::get<0,1>(b)},
        {bg::get<0,0>(b),bg::get<0,1>(b)}
	),
	ips);
    for (auto &p: ips)
	pointset.insert({bg::distance(p,base),p});

    const auto &first_point = (*pointset.cbegin()).second;
    const auto &last_point = (*pointset.rbegin()).second;
	
    if (pointset.size() == 1)
    {
	
	// one intersection: omit first half
	if (bg::within(base, b))
	  *out++ = segment_type(first_point,endpoint);
	else
	  *out++ = segment_type(base,first_point);
	  
    }else{
	// two intersection
	  *out++ = segment_type(base,first_point);
	  *out++ = segment_type(last_point,endpoint);
    }
        
}

int main(void)
{
   std::vector<segment_type> segments;
   for (int i=0; i < 100; i++)
   {
      segments.push_back(segment_type(random_point(),random_point()));
   }
   for (const auto &s: segments){
      std::cout<< "SEG:" << bg::wkt(s) << std::endl;
      
   }

   box_type b = random_box();
   std::cout << "BOX:" << bg::wkt(b) << std::endl;

   // subtract box from segmenta
   std::vector<segment_type> result;
   for (const auto &s: segments){
       cut_box_out_of_segment(s,b,std::back_inserter(result));

   }

   for (auto l:result)
      std::cout << "RESULT: " << bg::wkt(l) << std::endl;
      
   return 0;
}
