#ifndef _READ_TEXTURE_H_
#define _READ_TEXTURE_H_

// reads a PNG file
bool ReadPNG( const char * pFilename,
              unsigned int & rWidth,
              unsigned int & rHeight,
              unsigned char ** ppTexBuffer );

// reads a TGA file
bool ReadTGA( const char * pFilename,
              unsigned int & rWidth,
              unsigned int & rHeight,
              unsigned char ** ppTexBuffer );

// reads a rgb / rgba file
bool ReadRGB( const char * pFilename,
              unsigned int & rWidth,
              unsigned int & rHeight,
              unsigned char ** ppTexBuffer );

#endif // _READ_TEXTURE_H_