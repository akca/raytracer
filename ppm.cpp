#include "ppm.h"
#include <stdexcept>

void write_ppm(const char *filename, unsigned char **data, int width,
               int height) {
  FILE *outfile;

  if ((outfile = fopen(filename, "w")) == NULL) {
    throw std::runtime_error(
        "Error: The ppm file cannot be opened for writing.");
  }

  (void)fprintf(outfile, "P3\n%d %d\n255\n", width, height);

  unsigned char color;
  int arraySize = height / 4;

  for (int arrayIndex = 0; arrayIndex < 4; arrayIndex++) {

    for (int j = arrayIndex * arraySize, idx = 0;
         j < (arrayIndex + 1) * arraySize; ++j) {

      for (int i = 0; i < width; ++i) {
        for (int c = 0; c < 3; ++c, ++idx) {

          color = data[arrayIndex][idx];

          if (i == width - 1 && c == 2) {
            (void)fprintf(outfile, "%d", color);
          } else {
            (void)fprintf(outfile, "%d ", color);
          }
        }
      }

      (void)fprintf(outfile, "\n");
    }
  }
  (void)fclose(outfile);
}
