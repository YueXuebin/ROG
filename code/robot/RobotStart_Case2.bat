:start
@cd ../bin

@rem 循环登陆测试次数
@for /L %%a in (1,1,100) do @call :robot

@rem goto start

@goto end

:robot
@rem echo robot
@ping 127.0.0.1 -n 5 | find ""
@rem 同时登陆人数
@for /L %%a in (1,1,15) do @start Robot.exe -c 2 -i 127.0.0.1 -p 9527
@goto :eof

:end