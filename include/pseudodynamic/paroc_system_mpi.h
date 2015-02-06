/**
 *
 * Copyright (c) 2005-2012 POP-C++ project - GRID & Cloud Computing group, University of Applied Sciences of western Switzerland.
 * http://gridgroup.hefr.ch/popc
 *
 * @author Laurent Winkler
 * @date Jan 2015
 * @brief This files contains all global variables used by pseudodynamic version (=MPI). Previously contained in paroc_system.h
 *
 */

#ifndef _POPC_SYSTEM_MPI_H
#define _POPC_SYSTEM_MPI_H
#include "paroc_system.h"
#include "paroc_mutex.h"
#include <mpi.h>

/**
 * @class paroc_system_mpi
 * @brief System information, used by POP-C++ runtime. Specific to MPI
 * @author Tuan Anh Nguyen
 */

// Note: Since everything is static, we cannot use inheritance

class paroc_system_mpi {
public:

// if MPI

    static int current_free_process;

    static MPI::Intracomm popc_self;
    static bool is_remote_object_process;
    static bool mpi_has_to_take_lock;

    static paroc_condition mpi_unlock_wait_cond;
    static paroc_condition mpi_go_wait_cond;
// end if MPI
};
typedef paroc_system_mpi POPSystemMPI;

#endif