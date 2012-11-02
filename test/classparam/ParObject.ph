#ifndef _PAROBJECT_PH_
#define _PAROBJECT_PH_
#include "classdata.h"

parclass ParObject        
{

classuid(1001);

public:
	 ParObject() @{ od.node(1); od.executable("./ParObject.obj"); };
	 ParObject (paroc_string machine) @{ od.node(1); od.executable("./ParObject.obj"); };
   ParObject(float f) @{od.power(f);};
	~ ParObject ();

	seq async void SetData(ClassData data);
	seq sync ClassData GetData();
  seq sync void Get(ClassData &data);

private:
	ClassData theData;
};
#endif
