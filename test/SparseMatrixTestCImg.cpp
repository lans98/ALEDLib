#include <CImg.h>
#include <memory>
#include <vector>
#include <cstdint>
#include <fstream>
#include <cstring>
#include <iostream>

#include "SparseMatrix.hpp"

namespace img = cimg_library;
using spectrum = unsigned char;

img::CImg<spectrum> copy_greyscale(const img::CImg<spectrum>& img) {
  img::CImg<spectrum> grey_img(img.width(), img.height(), 1, 1, 0);

  spectrum r, g, b;
  spectrum avg = 0;

  cimg_forXY(img, x, y) {
    r = img(x, y, 0);
    g = img(x, y, 1);
    b = img(x, y, 2);
  
    avg = (r * 0.3) + (g * 0.3) + (b * 0.3);  
    grey_img(x, y, 0) = avg;  
  }

  return grey_img;
}

std::vector<int> generate_histogram(const img::CImg<spectrum>& img) {
  std::vector<int> histogram(256, 0);
  spectrum grey_value;

  cimg_forXY(img, x, y) {
    grey_value = img(x,y);  
    ++histogram[grey_value];
  }

  return histogram;
}

int otsu_threshold(const img::CImg<spectrum>& img) {
  std::vector<int> histo(generate_histogram(img));

  std::size_t no_pixels = img.width() * img.height();

  double sum = 0;
  for (int ii = 0; ii < histo.size(); ++ii) 
    sum += ii * histo[ii];

  double sumB;
  int wB;
  int wF;

  double var_max = 0;
  int threshold  = 0;

  int ii; 
  for (ii = 0; ii < histo.size(); ++ii) {
    wB += histo[ii];
    if (wB == 0) continue;

    wF += no_pixels - wB;
    if (wF == 0) continue;

    sumB += static_cast<double>(ii * histo[ii]);

    double mB = sumB / wB;
    double mF = (sum - sumB) / wF;

    double var_between = (double) wB * (double) wF * (mB - mF) * (mB - mF);

    if (var_between > var_max) {
      var_max   = var_between;
      threshold = ii;
    }
  }

  return threshold;
}

img::CImg<spectrum> copy_binary(const img::CImg<spectrum>& img) {
  img::CImg<spectrum> bin_img(img.width(), img.height(), 1, 1, 0);

  spectrum grey_value;
  spectrum bin_value;

  int threshold = otsu_threshold(img);

  cimg_forXY(img, x, y) {
    grey_value = img(x,y);

    bin_value = (grey_value > threshold ? 255 : 0);
  
    bin_img(x,y,0) = bin_value;
  }
  
  return bin_img;
}

void binarize(const std::string& filename) {
  img::CImg<spectrum> img(filename.c_str());

  auto bimg = copy_binary(copy_greyscale(img));
  
  aled::FastMatrix<spectrum, 0, uint16_t> compress(bimg.height(), bimg.width());
  spectrum temp;
  cimg_forXY(bimg, x, y) {
    temp = bimg(x,y,0);
    if (temp != 0) 
      compress.add(temp, y, x);
  }

  compress.write(filename + ".imgbin", false);
}

img::CImg<spectrum> readbin(const std::string& filename) {
  aled::FastMatrix<spectrum, 0, uint16_t> compress(0,0);
  compress.read(filename, false);

  aled::FastMatrix<spectrum, 0>::Size_t width, height;
  width  = compress.noCols();
  height = compress.noRows();

  img::CImg<spectrum> image(width, height, 1, 1, 0);

  spectrum temp;
  for (aled::FastMatrix<spectrum,0>::Size_t ii = 0; ii < height; ++ii) {
    for (aled::FastMatrix<spectrum,0>::Size_t jj = 0; jj < width; ++jj) {
      try {
        image(jj, ii, 0) = compress.get(ii, jj);
      } catch (std::exception& e) {
        image(jj, ii, 0) = compress.get_default();
      }
    }
  }

  return image;
}


int interactive_proccess() {
  std::cout << "1: load already binarized image\n"
            << "2: load image to binarize it\n"
            << "q: exit" << std::endl;

  std::string input;
  std::cin >> input;

  if (input.length() > 1) return 4;
  char a = *(input.c_str());

  while (std::cin.get());
    
  switch (a) {
    case '1': 
    {
      std::cout << "filename: ";
      std::string filename;
      std::cin >> filename;
      readbin(filename);
      return 1;
    }
    case '2': 
    {
      std::string filename;
      std::cout << "filename: ";
      std::cin >> filename;
      binarize(filename);
      return 2;
    }
    case 'q':
      return 3;      
    default:
      return 4;
  }
}



int main(int argc, char** argv) {
  if (argc == 1) {
    int result = interactive_proccess();
    while (result == 4)
      result = interactive_proccess();
  } else {
    if (strcmp(argv[1], "-r") == 0) {
      auto bimg = readbin(std::string(argv[2]));
      bimg.display();
    } else if (strcmp(argv[1], "-b") == 0) {
      binarize(std::string(argv[2]));
    } else {
      return ~(0);
    }
  } 

  return 0;
}
