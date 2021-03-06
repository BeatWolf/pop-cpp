/**
 *
 * Copyright (c) 2005-2012 POP-C++ project - GRID & Cloud Computing group, University of Applied Sciences of western
 *Switzerland.
 * http://gridgroup.hefr.ch/popc
 *
 * @author Valentin Clement
 * @date 2012/11/30
 * @brief Declaration of a base
 *
 *
 */

#ifndef _POP_GROUP_BROKER_FACTORY_H_
#define _POP_GROUP_BROKER_FACTORY_H_

#include "popc_group_broker.h"
#include "paroc_list.h"

#include <map>

typedef POPC_GroupBroker* (*initbrokerfunc_group)();
typedef bool (*ispackedfunc)(const char* objname);
typedef std::map<std::string, initbrokerfunc_group> popc_broker_map;

// Obsolete to be remove
typedef paroc_list<std::string> popc_list_string;

class POPC_GroupBrokerFactory {
public:
    POPC_GroupBrokerFactory(initbrokerfunc_group func, const char* name);
    static POPC_GroupBroker* create(const char* objname);
    static void list_all(popc_list_string& objlist);
    // static POPC_GroupBroker *Create(int *argc, char ***argv);
    // static void PrintBrokers(const char *abspath, bool longformat);

    bool test(const char* objname);

    static ispackedfunc
        check_if_packed;  // Since this method is created by the parser, we have to declare a fct pointer for it

private:
    static popc_broker_map* _internal_broker_map;
};

#endif /* _POPC_GROUP_BROKER_FACTORY_H_ */
