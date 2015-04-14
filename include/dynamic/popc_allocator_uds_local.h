/**
 *
 * Copyright (c) 2005-2012 POP-C++ project - GRID & Cloud Computing group, University of Applied Sciences of western Switzerland.
 * http://gridgroup.hefr.ch/popc
 *
 * @author Baptiste Wicht
 * @date 2015/03/27
 * @brief
 */

#ifndef POPC_ALLOCATOR_UDS_LOCAL_H_
#define POPC_ALLOCATOR_UDS_LOCAL_H_

#include "popc_allocator.h"

#include "paroc_od.h"

struct popc_allocator_uds_local : public POPC_Allocator {
    virtual std::string allocate(const std::string& objectname, const paroc_od& od);
    virtual paroc_combox* allocate_group(const std::string& objectname, const paroc_od& od, int nb);

    virtual POPC_Protocol get_protocol() {
        return POPC_Allocator::UDS;
    }
    virtual POPC_AllocationMechanism get_mechanism() {
        return POPC_Allocator::LOCAL;
    }
};

#endif
