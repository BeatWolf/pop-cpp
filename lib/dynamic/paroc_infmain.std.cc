/**
 *
 * Copyright (c) 2005-2012 POP-C++ project - GRID & Cloud Computing group, University of Applied Sciences of western Switzerland.
 * http://gridgroup.hefr.ch/popc
 *
 * @author Tuan Anh Nguyen
 * @date 2005/01/01
 * @brief "main" entry for the main program.
 *
 *
 */

#include "popc_intface.h"

#include "paroc_exception.h"
#include "paroc_buffer_factory_finder.h"
#include "paroc_broker.h"
#include "paroc_system.h"
#include "paroc_utils.h"
#include "popc_logger.h"

// Set processor
#include <sched.h>
#define error printf
#define panic printf
// #define debug(a,b,c) printf((b),(c),(a))

extern int parocmain(int, char **);

void SignalTerminate(int sig) {
    LOG_ERROR( "SIGNAL %d!!!!",sig);
    paroc_system::Finalize(false);
    exit(1);
}

void _paroc_atexit() {
    LOG_WARNING( "_paroc_atexit called !!!!");
    paroc_system::Finalize(false);
}

int main(int argc, char **argv) {

#ifdef UC_LINUX
    paroc_system::processor_set(0);
#endif

    paroc_system app;
    paroc_system::pop_current_local_address = 1;
    paroc_system::popc_local_mpi_communicator_rank = 0;

    int i;
    for(i=argc-1; i>=0; i--) {
        if(paroc_utils::isEqual(argv[i],"-initparoc")) {
            char **argv1=argv+i+1;
            int argc1=argc-i-1;
            if(!paroc_system::Initialize(&argc1, &argv1)) {
                LOG_WARNING("Initialization of parallel objects fail...");
                paroc_system::Finalize(false);
                return -1;
            }
            argv[i]=NULL;
            argc=i;
            break;
        }
    }
#ifndef __WIN32__
    popc_signal(popc_SIGKILL, SignalTerminate);
    popc_signal(popc_SIGQUIT, SignalTerminate);
    popc_signal(popc_SIGPIPE, popc_SIG_IGN);
#endif
    popc_signal(popc_SIGTERM, SignalTerminate);
    popc_signal(popc_SIGINT, SignalTerminate);

    if(i < 0) {
        LOG_DEBUG( "Call parocmain");
        int ret = parocmain(argc, argv);
        LOG_DEBUG( "End of parocmain");
        /**
        * POP-C++ on the K Computer
        * Terminate the MPI Communicators network by sending a termination message to the local MPI Communicator with rank 0
        */
#ifdef DEFINE_UDS_SUPPORT
        paroc_combox_factory* combox_factory = paroc_combox_factory::GetInstance();
        if(combox_factory == NULL) {
            paroc_exception::paroc_throw(POPC_NO_PROTOCOL, "POPCMain: combox_factory == NULL");
        }

        paroc_combox* allocating_combox = combox_factory->Create("uds");
        if(allocating_combox == NULL) {
            paroc_exception::paroc_throw(POPC_NO_PROTOCOL, "POPCMain: allocating_combox == NULL");
        }

        paroc_buffer* allocating_buffer = allocating_combox->GetBufferFactory()->CreateBuffer();

        char* local_address = new char[15];
        snprintf(local_address, 15, "uds_%d.0", paroc_system::popc_local_mpi_communicator_rank);

        if(!allocating_combox->Create(local_address, false) || !allocating_combox->Connect(local_address)) {
            paroc_exception::paroc_throw(POPC_NO_PROTOCOL, "POPCMain: allocating_combox->Create failed");
        }

        paroc_message_header header(0, 200001, INVOKE_SYNC, "_terminate");
        allocating_buffer->Reset();
        allocating_buffer->SetHeader(header);
        paroc_connection* connection = allocating_combox->get_connection();

        if(!allocating_buffer->Send((*allocating_combox), connection)) {
            paroc_exception::paroc_throw("allocating_buffer->Send failed");
        }
#endif
        return ret;
    }

    // TODO LW: See what to do with signal handling
    atexit(_paroc_atexit); // TODO LWK: Commented this: see if works

    try {
        int ret=parocmain(argc,argv);
        if(ret!=0)
            LOG_WARNING("main returned error code %d. Finalize method will kill all remaining objects", ret);
        app.Finalize(ret==0);
        return ret;
    } catch(std::exception &e) {
        LOG_ERROR("Exception in main: %s", e.what());
        paroc_system::Finalize(false);
        return -1;
    } catch(int e) {
        LOG_ERROR("Exception in main with code %d", e);
        paroc_system::Finalize(false);
        return -1;
    } catch(...) {
        LOG_ERROR("Unknown exception");
        paroc_system::Finalize(false);
    }
    return 1;
}
