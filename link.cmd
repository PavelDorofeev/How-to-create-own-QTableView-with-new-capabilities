@echo off
set CURDIR=%~dp0
echo %CURDIR%
::pause
::exit
::rmdir %CURDIR%\lib
::rmdir %CURDIR%\include

echo ---------------------------------------------------------------------------------------------------------------------------
mklink /j %CURDIR%\qp D:\\QtSDK1.2.1\\QtSources\\4.8.1\\include\\QtGui\\private\\qp
mklink /j %CURDIR%\qp_private D:\\QtSDK1.2.1\\QtSources\\4.8.1\\src\\gui\\itemviews\\qp_private
mklink /j %CURDIR%\QpTableView_test D:\\DEVELOPMENT\\QT4\\_MY__\\tests\\QpTableView_test
echo ---------------------------------------------------------------------------------------------------------------------------
pause
