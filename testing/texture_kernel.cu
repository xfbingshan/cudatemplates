/*
  The kernel is in a separate file to test if dependencies are handled correctly.
*/

__global__ void kernel(memdev_t::KernelData res)
{
  int x1 = threadIdx.x + blockDim.x * blockIdx.x;
  int y1 = threadIdx.y + blockDim.y * blockIdx.y;
  int x2 = res.size[0] - 1 - x1;
  int y2 = res.size[1] - 1 - y1;
  res.data[x1 + y1 * SIZE] = tex2D(tex, x2 + 0.5, y2 + 0.5);
}
