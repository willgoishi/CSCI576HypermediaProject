#ifndef IMAGE_H
#define IMAGE_H
#define MAX_PATH 260

#include <stdlib.h>
#include <stdio.h>
//#include <malloc.h>
#include <memory.h>
//#include <tchar.h>
#include <string.h>

class Image
{
private:
    int		Width;					// Width of Image
    int		Height;					// Height of Image
    char	ImagePath[MAX_PATH];	// Image location
    char*	Data;					// RGB data of the image

public:
    // Constructor
    Image();
    // Copy Constructor
    Image( Image *otherImage);
    // Destructor
    ~Image();

    // operator overload
    Image & operator= (const Image & otherImage);

    // Reader & Writer functions
    void	setWidth( const int w)  { Width = w; }
    void	setHeight(const int h) { Height = h; }
    void	setImageData( const char *img ) { Data = (char *)img; }
    void	setImagePath( const char *path) { strcpy(ImagePath, path); }
    int		getWidth() { return Width; }
    int		getHeight() { return Height; }
    char*	getImageData() { return Data; }
    char*	getImagePath() { return ImagePath; }

    // Input Output operations
    bool	ReadImage();
    bool	WriteImage();

    // Modifications
    bool	Modify();
};

#endif // IMAGE_H
