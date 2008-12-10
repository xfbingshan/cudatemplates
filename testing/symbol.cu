#include <cudatemplates/copy.hpp>
#include <cudatemplates/devicememorylinear.hpp>
#include <cudatemplates/hostmemoryheap.hpp>
#include <cudatemplates/symbol.hpp>


#define SIZE 1024


__constant__ float cdata1[SIZE], cdata2[SIZE];
__device__ float ddata1[SIZE], ddata2[SIZE];

Cuda::Symbol<float, 1> symbol(Cuda::Size<1>(1024), cdata1);


int
main()
{
  Cuda::Size<1> size(SIZE);
  Cuda::HostMemoryHeap<float, 1> host1(size), host2(size), host3(size);
  Cuda::DeviceMemoryLinear<float, 1> device1(size), device2(size);
  Cuda::Symbol<float, 1> csym1(size, cdata1), csym2(size, cdata2);
  Cuda::Symbol<float, 1> dsym1(size, ddata1), dsym2(size, ddata2);

  // init data:
  int seed = time(0);
  srand(seed);

  for(int i = SIZE; i--;)
    host1[i] = rand();

  // roundtrip:
  copy(csym1  , host1  );
  copy(device1, csym1  );
  copy(csym2  , device1);
  copy(host2  , csym2  );
  copy(dsym1  , host2  );
  copy(device2, dsym1  );
  copy(dsym2  , device2);
  copy(host3  , dsym2  );

#if 0
  // these produce a runtime error:
  // "cannot take the address of __constant__ data"
  float *cbuf1 = csym1.getBuffer();
  float *cbuf2 = csym2.getBuffer();
#endif

  float *dbuf1 = dsym1.getBuffer();
  float *dbuf2 = dsym2.getBuffer();

  // verify data:
  srand(seed);

  for(int i = SIZE; i--;)
    assert(host3[i] == rand());
}
