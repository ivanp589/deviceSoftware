#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>

typedef struct {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} RGBPixel;

void processImage(const char* jpegFilename) {
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    FILE* infile;
    JSAMPARRAY buffer;
    int row_stride;

    if ((infile = fopen(jpegFilename, "rb")) == NULL) {
        fprintf(stderr, "Error opening JPEG file: %s\n", jpegFilename);
        return;
    }

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    row_stride = cinfo.output_width * cinfo.output_components;
    buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);

    // Create a new file for saving the modified image
    FILE* outfile = fopen("output.jpg", "wb");
    if (outfile == NULL) {
        fprintf(stderr, "Error creating output file\n");
        jpeg_finish_decompress(&cinfo);
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        return;
    }

    // Initialize JPEG destination manager for writing the modified image
    struct jpeg_compress_struct cinfo_out;
    struct jpeg_error_mgr jerr_out;

    cinfo_out.err = jpeg_std_error(&jerr_out);
    jpeg_create_compress(&cinfo_out);
    jpeg_stdio_dest(&cinfo_out, outfile);

    // Set output image parameters
    cinfo_out.image_width = cinfo.output_width;
    cinfo_out.image_height = cinfo.output_height;
    cinfo_out.input_components = 3;  // RGB components
    cinfo_out.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo_out);
    jpeg_start_compress(&cinfo_out, TRUE);


printf("Image width: %d\n", cinfo.image_width);
printf("Image height: %d\n", cinfo.image_height);
printf("Number of color components: %d\n", cinfo.num_components);
printf("Color space: %d\n", cinfo.jpeg_color_space);
    // Process the pixel values and modify as needed
    while (cinfo.output_scanline < cinfo.output_height) {
        jpeg_read_scanlines(&cinfo, buffer, 1);
        int i;
        for (i = 0; i < cinfo.output_width; i++) {
            RGBPixel pixel;
            pixel.red = buffer[0][3 * i];
            pixel.green = buffer[0][3 * i + 1];
            pixel.blue = buffer[0][3 * i + 2];

            if (abs(pixel.red - pixel.green) <= 50 && abs(pixel.red - pixel.blue) <= 50) {
                // Set similar RGB values to black
                buffer[0][3 * i] = 0;
                buffer[0][3 * i + 1] = 0;
                buffer[0][3 * i + 2] = 0;
            }
        }

        jpeg_write_scanlines(&cinfo_out, buffer, 1);
    }

    jpeg_finish_compress(&cinfo_out);
    jpeg_destroy_compress(&cinfo_out);

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    fclose(infile);
    fclose(outfile);

    printf("Image processing complete.\n");
}







int main() {
    const char* jpegFilename = "input.jpg";
    const char* jpegFilename1 = "output.jpg";
	
    processImage(jpegFilename);
    
    return 0;
}
