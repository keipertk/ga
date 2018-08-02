#if !defined (GADEVICEPROPERTY_H)
#define GADEVICEPROPERTY_H

#include <stdio.h>
#include <mpi.h>

#ifdef CUDA_DEVICE
#include <cuda.h>
#include <cuda_runtime.h>
// int globalRank, localRank;

int getDeviceCount() {
  int deviceCount;
  cudaGetDeviceCount(&deviceCount);
  return deviceCount;
}

void gpu_device_bind()
{
  int global_rank, local_rank, global_size, local_size, rc, len, num_devices = 0;
  char errstr[MPI_MAX_ERROR_STRING];

  // Assuming MPI has already been initialized
  MPI_Comm_rank(MPI_COMM_WORLD, &global_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &global_size);
  MPI_Comm local_comm;
  MPI_Info info;
  MPI_Info_create(&info);

  MPI_Comm_split_type(MPI_COMM_WORLD, MPI_COMM_TYPE_SHARED, global_rank, info, &local_comm);

  local_rank = -1;
  MPI_Comm_rank(local_comm, &local_rank);
  // MPI_Comm_size(local_comm, &local_size);

  num_devices = getDeviceCount();

  // Default: Bind given rank/s per node to available GPUs on a node.
  // TODO: Selective binding of GPUs.
  // TODO: Bind multiple GPUs per rank?
  if(local_rank < num_devices) {
    cudaSetDevice(local_rank % num_devices);
  }
  else {
    cudaSetDevice(-1);
  }

  MPI_Comm_free(&local_comm);
  MPI_Info_free(&info);
}
#endif

#endif // GADEVICEPROPERTY_H
