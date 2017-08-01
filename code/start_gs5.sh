#
#

. ./utils.sh
. ./stop_gs5.sh


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
    echo "start gameserver 5"
    time=`date | awk '{print $1"_"$2"_"$3"_"$4}'`
    echo $time

    username=`users | awk '{print $1}'`
    #echo $username
    if [ "$username" = "root" ]
    then
        nohup ./../dumpwrapper.sh ./gameserver -i 5 > /dev/null 2>&1 &
    else
        nohup sudo ./../dumpwrapper.sh ./gameserver -i 5 > /dev/null 2>&1 &
    fi
    cd ..
}

start
sleep 1
. scan.sh
echo "ok!"
