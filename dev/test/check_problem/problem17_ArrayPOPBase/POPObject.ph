#ifndef POPOBJECT_PH_
#define POPOBJECT_PH_

#include "POPData.h"

parclass POPObject {
    classuid(1500);

public:
    POPObject() @{ od.search(0, 0, 0); };
    ~POPObject();

    sync seq void transferPOPBaseArray(int n, [in, out, size=n] POPData* d);
    sync seq void transferPOPBaseData(POPData d);

private:

};

#endif /*POPOBJECT_PH_*/
