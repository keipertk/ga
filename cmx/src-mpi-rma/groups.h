/**
 * Private header file for cmx groups backed by MPI_comm.
 *
 * The rest of the cmx group functions are defined in the public cmx.h.
 *
 * @author Bruce Palmer
 */
#ifndef _CMX_GROUPS_H_
#define _CMX_GROUPS_H_

#include <mpi.h>

#include "cmx.h"

typedef struct win_link {
  struct win_link *next;
  struct win_link *prev;
  MPI_Win win;
} win_link_t;

typedef struct group_link {
    struct group_link *next;
    MPI_Comm comm;
    MPI_Group group;
    win_link_t *win_list;
} cmx_igroup_t;

extern void cmx_group_init();
extern void cmx_group_finalize();
extern cmx_igroup_t* cmx_get_igroup_from_group(cmx_group_t group);
extern void cmx_igroup_add_win(cmx_group_t group, MPI_Win win);
extern void cmx_igroup_delete_win(cmx_group_t group, MPI_Win win);

#endif /* _CMX_GROUPS_H_ */
