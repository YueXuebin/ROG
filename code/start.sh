#
#

# 检查服务器是否已经开启
if [ -f "../../sjsy_server_run" ]; then
    echo "server already run"
    exit
fi

# 工具函数
. ./utils.sh
# 打包log文件
. ./log_pack.sh

start()
{ 
    echo start:
    cd bin
    pwd
    #创建log目录
    if [ -d ./log ] ; then
    	:
    else
    	mkdir ./log
    fi
    
    #开启gamerserver
    echo "start all server"
    time=`date | awk '{print $1"_"$2"_"$3"_"$4}'`
    echo $time

    username=`users | awk '{print $1}'`
    #echo $username
    if [ "$username" = "root" ]
    then
        nohup ./../dumpwrapper.sh ./centerserver > /dev/null 2>&1 &
        nohup ./../dumpwrapper.sh ./gameserver -i 1 > /dev/null 2>&1 &
        nohup ./../dumpwrapper.sh ./gameserver -i 2 > /dev/null 2>&1 &
        nohup ./../dumpwrapper.sh ./gameserver -i 3 > /dev/null 2>&1 &
        nohup ./../dumpwrapper.sh ./gameserver -i 4 > /dev/null 2>&1 &
        nohup ./../dumpwrapper.sh ./gateserver -i 1 > /dev/null 2>&1 &
        nohup ./../dumpwrapper.sh ./gateserver -i 2 > /dev/null 2>&1 &
    else
        nohup sudo ./../dumpwrapper.sh ./centerserver > /dev/null 2>&1 &
        nohup sudo ./../dumpwrapper.sh ./gameserver -i 1 > /dev/null 2>&1 &
        nohup sudo ./../dumpwrapper.sh ./gameserver -i 2 > /dev/null 2>&1 &
        nohup sudo ./../dumpwrapper.sh ./gameserver -i 3 > /dev/null 2>&1 &
        nohup sudo ./../dumpwrapper.sh ./gameserver -i 4 > /dev/null 2>&1 &
        nohup sudo ./../dumpwrapper.sh ./gateserver -i 1 > /dev/null 2>&1 &
        nohup sudo ./../dumpwrapper.sh ./gateserver -i 2 > /dev/null 2>&1 &
    fi
    cd ..

    touch ../../sjsy_server_run > /dev/null
}

start
sleep 1
. ./scan.sh
echo "ok!"
