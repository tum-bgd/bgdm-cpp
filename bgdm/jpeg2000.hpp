#ifndef JPEG2000_H_INC
#define JPEG2000_H_INC
#include <openjpeg.h>
#include<iostream>
#include<string>
#include<stdexcept>


struct ESRIWorldFileNorthed{
  // supports only northed images with a21 = a12 = 0
  double a11; // x component of pixel width
  double a21; // y component of pixel width
  double a12; // x component of pixel height
  double a22; // y component of pixel height (often negative)
  double b1; // x coordinate of top-left
  double b2; // y coordinate of top-left

  
  
  std::pair<int,int> to_image(double x, double y)
  {
    // subtract basepoint
    x -= b1; y -= b2;
    // divide by pixel size
    x /= a11;
    y /= a22;
    return std::make_pair(static_cast<int>(x), static_cast<int>(y));    
  }


  box_type as_box(int w, int h)
  {
    w--;
    h--;
    // assuming a21 == a12 == 0
    double dx = b1 - (a11/2);
    double dy = b2 - (a22/2);
    double cx = b1 + (a11/2) + w*a11;
    double cy = b2 + (a22/2) + h*a22;
    return box_type(point_type(std::min(dx,cx),std::min(dy,cy)),
		    point_type(std::max(dx,cx),std::max(dy,cy)));

  }

template<typename stream>
void from_stream(stream & s)
{
  
  s >> a11;
  s >> a21; // y component of pixel width
  s >> a12; // x component of pixel height
  s >> a22; // y component of pixel height (often negative)
  s >> b1; // x coordinate of top-left
  s >> b2; // y coordinate of top-left
  //std::cout << "base point: (" << b1 << "," << b2 << ") A="<< a11 << "," << a12 << ";" << a21 << "," << a22 << std::endl;
  if (fabs(a21) > 1e-9 || fabs(a12) > 1e-9)
    throw(std::runtime_error("ESRI World File Reader Exception: Expecting northed data, but codiagonal of affine map is non-zero."));
} 
  void from_file(std::string filename){
    std::ifstream ifs(filename);
    if (!ifs) {
      std::cerr << "Error: " << strerror(errno);
      throw(std::runtime_error("Unable to stream world data from <" + filename+">" ));
    }
    from_stream(ifs);
    ifs.close();
  }
};


class JPEG2000{
private:
  unsigned char *image = NULL;
  opj_codestream_info_v2_t JP2CodeStreamInfo;
  opj_image_t* pJP2Image = NULL;
  
public:
  int width;
 JPEG2000(){};
 
  void dump()
  {
    std::cout << pJP2Image-> x0 << std::endl;
    std::cout << pJP2Image-> x1 << std::endl;
    std::cout << pJP2Image->numcomps << std::endl;
    for (int c = 0 ; c < pJP2Image->numcomps; c++)
    {
      std::cout << pJP2Image->comps[c].w << "x";
      std::cout << pJP2Image->comps[c].h << "?" << pJP2Image->comps[c].prec<< std::endl;
      
      std::cout << "P:" <<  (size_t) pJP2Image->comps[c].data << std::endl;
    }
    std::cout << "---" << std::endl;
  }
  void getpixel(int y, int x, unsigned char &r, unsigned char &g, unsigned char &b)
  {
     if (image != NULL)
     {
	 auto *p = image + 3*((width*x)+y);
	 r = *(p++);
	 g = *(p++);
	 b = *p;
	 return;
     }
    
    // assuming 8 bit pixels, need to check somehwere!
    r = *(reinterpret_cast<unsigned char *>(pJP2Image->comps[0].data +  x + y * width));
    g = *(reinterpret_cast<unsigned char *>(pJP2Image->comps[1].data +  x + y * width));
    b = *(reinterpret_cast<unsigned char *>(pJP2Image->comps[2].data +  x + y * width));
    
  }

  int w; int h;
 
  void compress_rgb(){
	// assume 3 components, maybe assert
	// assume all same size
	 w = pJP2Image->comps[0].w;
	 h = pJP2Image->comps[0].h;
	std::cout << "Compressing to " << w << "x" << h  << " for " << w*h*3 << "bytes" << std::endl;
	image = new unsigned char[w*h*3];
	auto *p=image;
	auto *r = pJP2Image->comps[0].data;
	auto *g = pJP2Image->comps[1].data;
	auto *b = pJP2Image->comps[2].data;
	
	if (image == NULL) throw (std::runtime_error("Out of memory in compress_rgb"));
	for (int y =0; y < h; y ++)
	  for(int x = 0; x < w; x++)
	    {
               //
	       *(p++) = *(reinterpret_cast<unsigned char *> (r++));
	       *(p++) = *(reinterpret_cast<unsigned char *> (g++));
       	       *(p++) = *(reinterpret_cast<unsigned char *> (b++));

	    }
       // now, delete the funny image
       opj_image_destroy(pJP2Image);
  }

  std::pair<int,int> get_size(std::string filename)
  {
    // read the file without decoding the data
    opj_stream_t* pJP2Stream = NULL;
    opj_codec_t* pJP2Codec = NULL;
    opj_dparameters_t jp2dParams;
 
    pJP2Stream = opj_stream_create_default_file_stream(filename.c_str(), OPJ_TRUE);
    if(!pJP2Stream) throw(std::runtime_error("get_size: "));


    pJP2Codec = opj_create_decompress(OPJ_CODEC_JP2);

    if(!pJP2Codec)
    {
      opj_stream_destroy(pJP2Stream);
      opj_destroy_codec(pJP2Codec);
      throw(std::runtime_error("get_size: "));
}

opj_set_default_decoder_parameters(&jp2dParams);
if ( !opj_setup_decoder(pJP2Codec, &jp2dParams) )
{
    //(stderr, "ERROR -> opj_compress: failed to setup the decoder\n");
    opj_stream_destroy(pJP2Stream);
    opj_destroy_codec(pJP2Codec);
    
    throw(std::runtime_error("get_size: "));
}

if( !opj_read_header(pJP2Stream, pJP2Codec, &pJP2Image) || (pJP2Image->numcomps == 0))
{

    opj_stream_destroy(pJP2Stream);
    opj_destroy_codec(pJP2Codec);
    if (pJP2Image)
        opj_image_destroy(pJP2Image);
    throw(std::runtime_error("get_size: "));;
}
   
 auto width = pJP2Image-> x1-pJP2Image-> x0; 
 auto height = pJP2Image-> y1-pJP2Image-> y0; 
    opj_stream_destroy(pJP2Stream);
    opj_destroy_codec(pJP2Codec);
    if (pJP2Image)
        opj_image_destroy(pJP2Image);
 
 return std::make_pair(width,height);
    
  }

  bool load(std::string filename)
{
opj_stream_t* pJP2Stream = NULL;
opj_codec_t* pJP2Codec = NULL;
opj_dparameters_t jp2dParams;


 
/*OPJ_API OPJ_BOOL OPJ_CALLCONV opj_decode(opj_codec_t *p_decompressor,
        opj_stream_t *p_stream,
        opj_image_t *p_image);
*/


 
 pJP2Stream = opj_stream_create_default_file_stream(filename.c_str(), OPJ_TRUE);
if(!pJP2Stream)
{
    return false;
}

   pJP2Codec = opj_create_decompress(OPJ_CODEC_JP2);

if(!pJP2Codec)
{
    opj_stream_destroy(pJP2Stream);
    opj_destroy_codec(pJP2Codec);
    return false;
}

//register callbacks
//opj_set_info_handler(pJP2Codec, info_callback,00);
//opj_set_warning_handler(pJP2Codec, warning_callback,00);
//opj_set_error_handler(pJP2Codec, error_callback,00);

opj_set_default_decoder_parameters(&jp2dParams);
if ( !opj_setup_decoder(pJP2Codec, &jp2dParams) )
{
    //(stderr, "ERROR -> opj_compress: failed to setup the decoder\n");
    opj_stream_destroy(pJP2Stream);
    opj_destroy_codec(pJP2Codec);
    return false;
}

if( !opj_read_header(pJP2Stream, pJP2Codec, &pJP2Image) || (pJP2Image->numcomps == 0))
{

    opj_stream_destroy(pJP2Stream);
    opj_destroy_codec(pJP2Codec);
    if (pJP2Image)
        opj_image_destroy(pJP2Image);
    return false;
}

opj_codestream_info_v2_t *pJP2CodeStreamInfo = NULL;

pJP2CodeStreamInfo = opj_get_cstr_info(pJP2Codec);
if(pJP2CodeStreamInfo)
{
    JP2CodeStreamInfo = *pJP2CodeStreamInfo;
    opj_destroy_cstr_info(&pJP2CodeStreamInfo);
}
else
  return false;

 width = pJP2Image-> x1-pJP2Image-> x0; 
 opj_decode(pJP2Codec,pJP2Stream,pJP2Image);
 std::cout << "Successfully loading " << filename << std::endl;
 return true;
}

};



#endif
