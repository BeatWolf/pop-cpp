/**
 * File : appservice.cc
 * Author : Tuan Anh Nguyen
 * Description : implementation of the application scope service
 * Creation date : -
 *
 * Modifications :
 * Authors      Date            Comment
 * clementval   2010/11/08  Add the generation of the POP Application Unique ID and Getter for this ID.
 */

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <string>
#include <locale>
#include "appservice.ph"

// #include "jobmgr.ph"
#include "popc_logger.h"

AppCoreService::AppCoreService(const std::string& challenge, bool daemon, const std::string& codelocation)
    : pop_service_base(challenge),
      CodeMgr(challenge),
      RemoteLog(challenge),
      ObjectMonitor(challenge),
      BatchMgr(challenge) {
    /**
      * ViSaG : clementval
      * Generate the POP Application Unique ID
      */
    std::string tmpChallenge = challenge;
    time_t now = time(NULL);
    std::string ip = pop_system::GetIP();
    char id[100];
    std::string tmp(tmpChallenge.c_str());
    std::locale loc;
    const std::collate<char>& coll = std::use_facet<std::collate<char>>(loc);
    long hash = coll.hash(tmp.data(), tmp.data() + tmp.length());
    if (hash < 0) {
        hash = hash * -1;
    }
    long int timenow = now - 0;
    snprintf(id, sizeof(id), "POPAPPID_%ld_%ld_%s_%d", timenow, hash, ip.c_str(), popc_getpid());
    _popcAppId = id;
    /* ViSaG */

    if (daemon) {
        Start();
    }
    LoadAddOn();

    LOG_DEBUG("Create AppCoreService");
}

AppCoreService::~AppCoreService() {
    /* Commented LWK: This raises errors and seems pointless here
try {
    // Try to end the application on job mgr
    JobMgr jm(pop_system::jobservice);
    jm.ApplicationEnd(_popcAppId, true);
} catch(std::exception &e) {
    LOG_WARNING("Exception while destroying JobMgr: %s", e.what());
}
*/

    for (auto& t : servicelist) {
        t.name = "";
        try {
            t.service->Stop(mychallenge);
        } catch (std::exception& e) {
            LOG_WARNING("Exception while stopping service: %s", e.what());
        }
        delete t.service;
    }

    LOG_DEBUG("Destroyed AppCoreService");
}

bool AppCoreService::QueryService(const std::string& name, pop_service_base& service) {
    if (name.empty()) {
        return false;
    }

    for (auto& t : servicelist) {
        if (pop_utils::isncaseEqual(name, t.name)) {
            service = (*t.service);
            return true;
        }
    }
    return false;
}

bool AppCoreService::QueryService(const std::string& name, pop_accesspoint& service) {
    if (name.empty()) {
        return false;
    }

    for (auto& t : servicelist) {
        if (pop_utils::isncaseEqual(name.c_str(), t.name.c_str())) {
            service = t.service->GetAccessPoint();
            return true;
        }
    }

    return false;
}

bool AppCoreService::RegisterService(const std::string& name, const pop_service_base& newservice) {
    if (name.empty()) {
        return false;
    }

    ServiceEntry t;
    try {
        t.service = new pop_service_base(newservice);
        t.name = name;
    } catch (std::exception& e) {
        LOG_WARNING("Exception while creating service: %s", e.what());
        return false;
    }
    servicelist.push_back(t);
    return true;
}

bool AppCoreService::UnregisterService(const std::string& name) {
    if (name.empty()) {
        return false;
    }

    auto pos = servicelist.begin();
    while (pos != servicelist.end()) {
        auto old = pos;
        auto& t = *pos++;
        if (pop_utils::isncaseEqual(name.c_str(), t.name.c_str())) {
            delete t.service;
            t.name = "";
            servicelist.erase(pos);
            return true;
        }
    }
    return false;
}

void AppCoreService::LoadAddOn() {
    char fname[1024];
    char* popdir;
    if ((popdir = getenv("POPC_APPSERVICE_CONF")) != NULL) {
        strcpy(fname, popdir);
    } else if ((popdir = getenv("POPC_LOCATION")) == NULL) {
        return;
    } else {
        snprintf(fname, sizeof(fname), "%s/etc/appservice.conf", popdir);
    }

    FILE* f = fopen(fname, "r");
    if (f == NULL) {
        return;
    }

    char buf[1024];
    char exec[1024];
    char service[1024];

    while (fgets(buf, 1023, f) != NULL) {
        char* objfile = buf;
        while (isspace(*objfile)) {
            objfile++;
        }
        if (*objfile == '#' || *objfile == 0) {
            continue;
        }

        char* tmp = strstr(objfile, "-object=");
        if (tmp == NULL) {
            LOG_DEBUG("No addon service name specified: %s", objfile);
            continue;
        }
        pop_accesspoint ap;
        pop_accesspoint jobmgr;
        pop_od od;  // Note : the od is empty !
        snprintf(exec, sizeof(exec), "%s -constructor", objfile);
        if (pop_interface::LocalExec(NULL, exec, NULL, jobmgr, GetAccessPoint(), &ap, 1, od) != 0) {
            LOG_DEBUG("Fail to start the add-on [%s]", buf);
            continue;
        }
        try {
            pop_service_base s(ap);
            s.Start(mychallenge);
        } catch (std::exception& e) {
            LOG_WARNING("Can not connect to %s: %s", service, e.what());
            continue;
        }
        if (tmp != NULL && sscanf(tmp + 8, "%s", service) == 1) {
            LOG_DEBUG("Service: %s", service);
            RegisterService(service, ap);
        }
    }
    fclose(f);
}

/**
 * ViSaG : clementval
 * Getter for the POP Application Unique ID
 * @return a std::string containing the POPAppID
 */
std::string AppCoreService::GetPOPCAppID() {
    return _popcAppId;
}

int popc_appservice_log(const char* format, ...) {
    char* tmp = getenv("POPC_TEMP");
    char logfile[256];
    if (tmp != NULL) {
        snprintf(logfile, sizeof(logfile), "%s/popc_appservice_log", tmp);
    } else {
        strcpy(logfile, "/tmp/pop_appservice.log");
    }

    FILE* f = fopen(logfile, "a");
    if (f == NULL) {
        return 1;
    }
    time_t t = time(NULL);
    fprintf(f, "%s", ctime(&t));
    va_list ap;
    va_start(ap, format);
    vfprintf(f, format, ap);
    fprintf(f, "%s", "\n");
    va_end(ap);
    fclose(f);
    return 0;
}
