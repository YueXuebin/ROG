@set ip=192.168.0.106
@set port=9542
@set posx=-500
@set posy=500
@echo �˿�=%port%

@for /L %%a in (1,1,35) do @call :robot 5
@for /L %%a in (1,1,5) do @call :robot 1

@goto end

@rem ����������
:robot
@rem echo robot
@set arg1=%1
@start Robot.exe --case %arg1% --ip %ip% --port %port% --x %posx% --y %posy%
@goto :eof


:end
