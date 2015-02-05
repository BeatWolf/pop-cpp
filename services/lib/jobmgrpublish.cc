#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "jobmgr.ph"

int main(int argc, char **argv) {
    char master[256];
    char slaver[256];

    if(paroc_utils::checkremove(&argc,&argv,"-help")!=NULL) {
        LOG_INFO("Usage: jobmgrpublish [-help] [jobmgr contact | jobmgr host]");
        return 0;
    }

    if(argc<=1) {
        strcpy(master,paroc_system::GetHost());
    } else {
        strcpy(master,argv[1]);
    }

    if(strchr(master,':')==NULL) {
        strcat(master,":2711");
    }

    try {
        paroc_accesspoint job;
        job.SetAccessString(master);
        JobMgr info(job);

        LOG_INFO("Enter a child node contact:");
        while(1) {
            if(scanf("%s",slaver)!=1) {
                break;
            }
            if(strchr(slaver,':')==NULL) {
                strcat(slaver,":2711");
            }
            paroc_accesspoint node;
            node.SetAccessString(slaver);
            info.RegisterNode(node);
        }
    } catch(paroc_exception *e) {
        LOG_WARNING("Exception occured. Extra info: %s", (const char *)e->Extra());
        delete e;
    }
    return 0;
}
