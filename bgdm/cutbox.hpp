#ifndef CUTBOX_HPP_INC
#define CUTBOX_HPP_INC


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
#endif
