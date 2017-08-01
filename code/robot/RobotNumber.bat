:start
@cls
@set /A robotCount=0
@for /f "skip=1 delims=" %%i in ('tasklist /FI "imagename eq Robot.exe"') do @call :CountRobot %%i
@echo robot=%robotCount%
@ping 127.0.0.1 -n 3 | find ""
@goto start

:CountRobot
@rem echo %1%
@if "%1%"=="Robot.exe" set /A robotCount=robotCount+1
@rem echo %robotCount%

:end