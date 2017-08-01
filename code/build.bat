@if "%VS90COMNTOOLS%" == "" goto error
@call "%VS90COMNTOOLS%\vsvars32.bat"

@devenv rog-server.sln /Clean Debug
@devenv rog-server.sln /Build Debug

@copy dependence\mysql\lib\win\libmysql.dll bin\libmysql.dll
@copy dependence\curl\lib\win\libcurl.dll bin\libcurl.dll
@copy %QTDIR%\bin\QtCored4.dll bin\QtCored4.dll
@copy %QTDIR%\bin\QtGuid4.dll bin\QtGuid4.dll

@goto end
:error
@echo error

:end


pause
