@ECHO OFF
TASKLIST|FIND "CenterServer.exe"
IF ERRORLEVEL 1 (
ECHO "not find and start..."
) ELSE (
ECHO "find and kill..."
TASKKILL /F /IM CenterServer.exe /T
)

TASKLIST|FIND "LogicServer.exe"
IF ERRORLEVEL 1 (
ECHO "not find and start..."
) ELSE (
ECHO "find and kill..."
TASKKILL /F /IM LogicServer.exe /T
)

TASKLIST|FIND "LogicDB.exe"
IF ERRORLEVEL 1 (
ECHO "not find and start..."
) ELSE (
ECHO "find and kill..."
TASKKILL /F /IM LogicDB.exe /T
)

TASKLIST|FIND "GateServer.exe"
IF ERRORLEVEL 1 (
ECHO "not find and start..."
) ELSE (
ECHO "find and kill..."
TASKKILL /F /IM GateServer.exe /T
)

TASKLIST|FIND "LogicManager.exe"
IF ERRORLEVEL 1 (
ECHO "not find and start..."
) ELSE (
ECHO "find and kill..."
TASKKILL /F /IM LogicManager.exe /T
)

::延迟一段时间
ping 127.0.0.1 
START CenterServer.exe
START LogicDB.exe
ping 127.0.0.1
START LogicManager.exe
ping 127.0.0.1
START LogicServer.exe
START GateServer.exe
START LoginServer.exe
ECHO "+_+ Service statr..."