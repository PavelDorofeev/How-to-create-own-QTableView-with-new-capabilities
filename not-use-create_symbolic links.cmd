@echo off
set CURDIR=%~dp0
echo %CURDIR%

echo ---------------------------------------------------------------------------------------------------------------------------
::mklink /j %CURDIR%\logging_system ..\..\..\my_lib\logging_system
mklink /j %CURDIR%\QpTableView_test ..\QpTableView_test
::mklink /j %CURDI:%\qp ..\..\..\my_lib\qp


echo ---------------------------------------------------------------------------------------------------------------------------
pause
