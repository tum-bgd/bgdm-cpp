#include "png.hpp"



int main(void){
   unsigned char data[100*100*4];
   for (size_t i=0; i < 100; i++)
     for (size_t j=0; j < 100; j++)
     {
	  unsigned char *p = data + 400*i + 4*j;
	  *p++ = 255-j;
	  *p++ = 50+j;
	  *p++ = 42;
	  *p++ = 127;
  }
  for (int i=20; i < 40; i++)
    for (int j =20; j<40; j++)
    {
	  unsigned char *p = data + 400*i + 4*j;
	  *p++ = 255;
	  *p++ = 0;
	  *p++ = 0;
	  *p=255;

    }
  // rgba data ready

  writepng("/var/www/html/test.png",data,100,100);
  return 0;

}
