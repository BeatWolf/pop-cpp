/**
 *
 * Copyright (c) 2005-2012 POP-C++ project - GRID & Cloud Computing group, University of Applied Sciences of western
Switzerland.
 * http://gridgroup.hefr.ch/popc
 *
 * @author Tuan Anh Nguyen
 * @date 2005/01/01
 * @brief system stuffs and declarations used by the runtime
 *
 *
Modified by L.Winkler (2008-2009) for Version 1.3
            P.Kuonen February 2010 (POPC_Host_Name attribut)
            for version 1.3m (see comments 1.3m)
 */

#ifndef _POP_SYSTEM_H
#define _POP_SYSTEM_H
#include <pop_accesspoint.h>

#ifndef __WIN32__
#include <net/if.h>
#include <ifaddrs.h>
#endif
#define DEFAULTPORT 2711

// For set processor
#include <sched.h>

// Added by clementval for COUT support
#include <iostream>
#include <sstream>
// End of add

class AppCoreService;

/**
 * @class pop_system
 * @brief System information, used by POP-C++ runtime.
 * @author Tuan Anh Nguyen
 */
class pop_system {
public:
    pop_system();
    ~pop_system();

    static bool Initialize(int* argc, char*** argv);  // only for the main...
    static void Finalize(bool normalExit);  // only for the main...

    /**
     * @brief Returns host of node in string format
     */
    static std::string GetHost();

    /**
     * @brief Returns IP of node in string format
     * Note: checks POPC_IP, then ip from POPC_IFACE, then ip from GetDefaultInterface()
     */
    static std::string GetIP();

    /**
     * @brief Returns IP of node
     * @param hostname
     * @param iplist
     * @param listsize
     */
    static int GetIP(const char* hostname, int* iplist, int listsize);

    /**
     * @brief Returns IP of node
     * @param iplist returned IP in integer format
     * @param listsize Max nb of IPs returned
     * Note : calls GetIP()
     */
    static int GetIP(int* iplist, int listsize);

    /**
     * @brief Returns local IP on interface
     * @param iface interface
     * @param str_ip returned IP
     * @return true if success
     */
    static bool GetIPFromInterface(std::string& iface, std::string& str_ip);

    /**
     * @brief Returns the default interface
     * @return interface
     * Note: Returned value is based on default gateway
     */
    static std::string GetDefaultInterface();

    /**
     * @brief Sets object execution on a given CPU
     */
    static void processor_set(int cpu);

public:
    static pop_accesspoint appservice;
    static pop_accesspoint jobservice;
    static int pop_current_local_address;
    static int popc_local_mpi_communicator_rank;
    // static pop_accesspoint popcloner;
    static std::string platform;
    static std::ostringstream _popc_cout;

private:
    static std::string pop_hostname;  // V1.3m

    static AppCoreService* CreateAppCoreService(char* codelocation);
    static AppCoreService* mgr;
    static std::string challenge;
};
typedef pop_system POPSystem;

#endif
