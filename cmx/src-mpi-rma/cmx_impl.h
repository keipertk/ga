#ifndef COMEX_IMPL_H_
#define COMEX_IMPL_H_

#include <semaphore.h>

#include <stdio.h>

#include <mpi.h>

#include "groups.h"

#define COMEX_MAX_NB_OUTSTANDING 8
#define SHM_NAME_SIZE 20

typedef struct {
    MPI_Comm world_comm;
    int rank;
    int size;
} local_state;

typedef struct {
  MPI_Win win;
  MPI_Comm comm;
  cmxInt bytes;
  int rank;
  void *buf;
} _cmx_handle;

typedef struct {
  MPI_Request request;
  MPI_Win win;
  int active;
#ifdef USE_MPI_FLUSH_LOCAL
  int remote_proc;
#endif
} _cmx_request;

extern local_state l_state;

#define DEBUG 0
#define COMEX_STRINGIFY(x) #x
#ifdef NDEBUG
#define COMEX_ASSERT(WHAT) ((void) (0))
#else
#define COMEX_ASSERT(WHAT) \
  ((WHAT) \
   ? (void) (0) \
   : comex_assert_fail (COMEX_STRINGIFY(WHAT), __FILE__, __LINE__, __func__))
#endif

  static inline void comex_assert_fail(
      const char *assertion,
      const char *file,
      unsigned int line,
      const char *function)
{
  fprintf(stderr, "[%d] %s:%u: %s: Assertion `%s' failed",
      l_state.rank, file, line, function, assertion);
  fflush(stderr);
#if DEBUG
  printf("[%d] %s:%u: %s: Assertion `%s' failed",
      l_state.rank, file, line, function, assertion);
#endif
  comex_error("comex_assert_fail", -1);
}
#endif /* COMEX_IMPL_H_ */
