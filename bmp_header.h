/* Tells the compiler not to add padding for these structs. This may
   be useful when reading/writing to binary files.
   http://stackoverflow.com/questions/3318410/pragma-pack-effect
*/
#pragma pack(1)

typedef struct bmp_fileheader
{
    unsigned char  signature1; /* 'B' */
    unsigned char  signature2; /* 'M' */
    unsigned int   file_size; /* File's size */
    unsigned short reserved1;
    unsigned short reserved2;
    unsigned int   offset; /* Offset to the start of image data */
} BITMAPFILEHEADER;

typedef struct bmp_infoheader
{
    unsigned int   size; /* Size of the info header - 40 bytes */
    signed int     width; /* Width of the image */
    signed int     height; /* Height of the image */
    unsigned short planes;
    unsigned short bit_count;
    unsigned int   compression;
    unsigned int   image_size; /* Size of the image data */
    int            x_pixels_per_meter;
    int            y_pixels_per_meter;
    unsigned int   colors_used;
    unsigned int   colors_important;
} BITMAPINFOHEADER;

#pragma pack()
