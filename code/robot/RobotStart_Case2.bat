:start
@cd ../bin

@rem ѭ����½���Դ���
@for /L %%a in (1,1,100) do @call :robot

@rem goto start

@goto end

:robot
@rem echo robot
@ping 127.0.0.1 -n 5 | find ""
@rem ͬʱ��½����
@for /L %%a in (1,1,15) do @start Robot.exe -c 2 -i 127.0.0.1 -p 9527
@goto :eof

:end