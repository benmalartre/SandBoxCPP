#include <iostream>
#include <string>
#include <memory>
#include <vector>

#ifdef _WIN32
#include "Windows.h"
#endif

#define STROKE_WIDTH          7
#define STROKE_STYLE          0
#define BACKGROUND_COLOR      -10172161 // RGBA(255, 200, 100, 255)
#define STROKE_COLOR_DEFAULT  -986896   // RGBA(240, 240, 240, 255)
#define STROKE_COLOR_SELECTED -12566464 // RGBA(64, 64, 64, 255)
#define STROKE_COLOR_DISABLED -6908266  // RGBA(150, 150, 150, 255)
#define FILL_COLOR_DEFAULT    -1644826  // RGBA(230, 230, 230, 255)
#define FILL_COLOR_SELECTED   -10461088 // RGBA(96, 96, 96, 255)
#define FILL_COLOR_DISABLED   -8355712  // RGBA(128, 128, 128, 255)
#define BLACK_COLOR           -14671840 // RGBA(32, 32, 32, 255)
#define WHITE_COLOR           -2171170  // RGBA(222, 222, 222, 255)
#define ORANGE_COLOR          -16736001 // RGBA(255, 160, 0, 255)
#define RED_COLOR             -16776961 // RGBA(255, 120, 120, 255)
#define GREEN_COLOR           -16711936 // RGBA(120, 255, 120, 255)
#define BLUE_COLOR            -34696    // RGBA(120, 120, 255, 255)

#ifdef WIN32
static const std::string iconLibPath = "Icons.dll";
#endif
#ifdef __linux__
static const std::string iconLibPath = "./Icons.so";
#endif
#ifdef __APPLE__
static const std::string iconLibPath = "./Icons.dylib";
#endif


typedef void(*BuildIconInMemory)(void* memory, int resolution, int icon, int fill, int stroke, int thickness);


static bool writeTga(const char* file, char* pixels, size_t resolution)
{
  FILE* f = fopen(file, "wb");
  if (f == NULL)
    return false;

  uint8_t lhs_res = resolution & 0xFFFF;
  uint8_t rhs_res = resolution >> 8;

  uint8_t targaheader[18] = {
    0,0,2,0,0,0,0,0,0,0,0,0,
    lhs_res, rhs_res,
    lhs_res, rhs_res,
    (uint8_t)(32), 0x20
  };

  // write header
  if (fwrite(targaheader, 18, 1, f) != 1)
  {
    fclose(f);
    return false;
  }

  // write scanlines
  std::vector<uint8_t> buffer(resolution * 4);
  for (int i = 0; i < resolution; i++)
  {
    uint8_t* dest = buffer.data();
    for (int j = 0; j < resolution; j++)
    {
      dest[0] = *(pixels++);
      dest[1] = *(pixels++);
      dest[2] = *(pixels++);
      dest[3] = *(pixels++);
      dest += 4;
    }
    if (fwrite(buffer.data(), resolution * 4, 1, f) != 1)
    {
      fclose(f);
      return false;
    }
  }

  fclose(f);
  return true;
}

void print(char* memory, int resolution)
{
  int counter = 0;
  int pixel_offset = 4;
  int line_offset = resolution * pixel_offset;
  for (int y = resolution - 1; y >= 0; --y) {
    for (int x = 0; x < resolution; ++x) {
      std::cout << "(" <<
        static_cast<unsigned>(memory[y * line_offset + x * pixel_offset])  << "," <<
        static_cast<unsigned>(memory[y * line_offset + x * pixel_offset + 1])  << "," <<
        static_cast<unsigned>(memory[y * line_offset + x * pixel_offset + 2])  << "," <<
        static_cast<unsigned>(memory[y * line_offset + x * pixel_offset + 3])  << "),";
      ++counter;
      if (counter >= 8) {
        std::cout << std::endl;
        counter = 0;
      }
    }
  }
}

int main()
{
  std::wstring stemp = std::wstring(iconLibPath.begin(), iconLibPath.end());
  HINSTANCE library = LoadLibrary((LPCWSTR)stemp.c_str());

  if (!library) {
    std::cout << "could not load the dynamic library" << std::endl;
    return EXIT_FAILURE;
  }

  // resolve function address here
  BuildIconInMemory func = (BuildIconInMemory)GetProcAddress(library, "BuildIconInMemory");
  if (!func) {
    std::cout << "could not locate the function !" << std::endl;
    return EXIT_FAILURE;
  }
  size_t resolution = 128;
  char* memory = new char[resolution * resolution * 4];
  memset(memory, 0, resolution * resolution * 4 * sizeof(char));
  writeTga("before.tga", memory, resolution);
  func(memory, resolution, 0, FILL_COLOR_DEFAULT, STROKE_COLOR_DEFAULT, STROKE_WIDTH);
  writeTga("after.tga", memory, resolution);
  return EXIT_SUCCESS;
}