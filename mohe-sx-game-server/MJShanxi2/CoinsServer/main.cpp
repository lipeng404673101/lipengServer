#include "Work.h"
#include "BaseCommonFunc.h"

#if _LINUX
#include "../LBase/Func.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#else
#include "MiniDump.h"
#endif
extern "C" { FILE __iob_func[3] = { *stdin,*stdout,*stderr }; }
int main(int argc, char *argv[])
{
#if _LINUX
	char strCurPath[512] = {0};
	getcwd(strCurPath, 512);
	chdir(strCurPath);
	SetCoreFileUnlimit();
	SetSockFileSize(0);
	if(argc == 1) daemonize(strCurPath);
	int nRet = WriteProcessPID(strCurPath);
	if(nRet < 0) exit(0);
#else
	MiniDumper minDump(true);
	disableConsoleCloseButton();
#endif

	gWork.Init();
	gWork.Start();
	gWork.Join();
	return 0;
}