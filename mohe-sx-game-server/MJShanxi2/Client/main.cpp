#include "LBase.h"
#include "Work.h"

int main()
{
	gWork.Init();
	gWork.Start();
	gWork.Join();
	return 0;
}