#关闭所有gameserver

. utils.sh

kill()
{
    PIDS=`find_pid_by_name $1`
    #echo $PIDS

    username=`users | awk '{print $1}'`
    #echo $username
    for id in $PIDS
    do
        echo "kill "$1" pid="$id
        if [ "$username" = "root" ]
        then
            /bin/kill -s 2 $id
        else
            sudo kill -s 2 $id
        fi
        sleep 1
    done
}

stop()
{
    echo stop:
    # 删除运行标记文件
    if [ -f "../../sjsy_server_run" ]; then
        rm ../../sjsy_server_run > /dev/null
    fi

    kill gateserver
    kill flashserver
    kill timerserver
    kill gameserver
    kill centerserver

    return
}

stop

. scan.sh
