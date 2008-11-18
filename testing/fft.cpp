/* 
  Cuda Templates.

  Copyright (C) 2008 Institute for Computer Graphics and Vision,
                     Graz University of Technology
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <sys/time.h>

#include <cstdlib>
#include <iostream>

#include <cudatemplates/copy.hpp>
#include <cudatemplates/cufft.hpp>
#include <cudatemplates/devicememorylinear.hpp>
#include <cudatemplates/hostmemoryheap.hpp>

using namespace std;


const size_t SIZE    =   1024;  // image size
const int    COUNT   = 100000;  // number of FFTs to perform
const float  EPSILON =   1e-6;  // error threshold


double
operator-(const struct timeval &t1, const struct timeval &t2)
{
  return (t1.tv_sec - t2.tv_sec) + (t1.tv_usec - t2.tv_usec) * 1e-6;
}

int
main()
{
  int err = 0;

  try {
    Cuda::HostMemoryHeap1D    <Cuda::FFT::real>    data1_h(SIZE);
    Cuda::DeviceMemoryLinear1D<Cuda::FFT::real>    data1_g(SIZE);
    Cuda::DeviceMemoryLinear1D<Cuda::FFT::complex> data_fft_g(SIZE / 2 + 1);
    Cuda::DeviceMemoryLinear1D<Cuda::FFT::real>    data2_g(SIZE);
    Cuda::HostMemoryHeap1D    <Cuda::FFT::real>    data2_h(SIZE);

    // allocate memory:
    Cuda::FFT::Plan<Cuda::FFT::real, Cuda::FFT::complex, 1> plan_r2c_1d(data1_g.size);
    Cuda::FFT::Plan<Cuda::FFT::complex, Cuda::FFT::real, 1> plan_c2r_1d(data1_g.size);

    // create random data:
    for(int i = SIZE; i--;)
      (data1_h.getBuffer())[i] = rand() / (float)RAND_MAX;

    // copy data to device memory:
    copy(data1_g, data1_h);

    // execute FFT and measure performance:
    struct timeval t1, t2;
    gettimeofday(&t1, 0);

    for(int i = COUNT; i--;) {
      plan_r2c_1d.exec(data1_g, data_fft_g);
      plan_c2r_1d.exec(data_fft_g, data2_g);
    }

    gettimeofday(&t2, 0);
    double t = t2 - t1;
    cout
      << "total time: " << t << " seconds\n"
      << "FFTs per second (" << SIZE << 'x' << SIZE << ", forward and inverse): " << (COUNT / t) << endl;

    // copy data to host memory:
    copy(data2_h, data2_g);

    // verify results:
    for(int i = SIZE; i--;) {
      float d = (data2_h.getBuffer())[i] / SIZE - (data1_h.getBuffer())[i];

      if(fabs(d) > EPSILON) {
	cerr << "FFT failed\n";
	return 1;
      }
    }
  }
  catch(const exception &e) {
    cerr << e.what();
    err = 1;
  }

  return err;
}
