


#include "integer.ph"
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
	try
	{
	  printf("OOOOOOOOOO : Starting integer example application\n");
		// Create 2 Integer objects
		Integer o1;
		Integer o2;

		// Set values
		/*o1.Set(1); o2.Set(2);

      cout << "o1="<< o1.Get() << "; o2=" << o2.Get() << popcendl;

		cout<<"Add o2 to o1"<< popcendl;
		o1.Add(o2);
		cout << "o1=o1+o2; o1=" << o1.Get() << popcendl;*/

	} catch (POPException *e) {
		cout << "Exception occurs in application :" << popcendl;
		e->Print();
		delete e;
		return -1;
	}
	return 0;
}
