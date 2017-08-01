
. ./utils.sh

#关闭所有gameserver
kill()
{
    PIDS=`find_pid_by_name $1`
    #echo $PIDS

    username=`users | awk '{print $1}'`
    do_once="0"
    #echo $username
    for id in $PIDS
    do
        # 结束进程
        echo "kill "$1" pid="$id
        if [ "$username" = "root" ]
        then
            /bin/kill -s 2 $id
        else
            sudo kill -s 2 $id
        fi
        do_once="1"
    done
   
    if [ "$do_once" = "1" ]
    then
        sleep 1
    fi
}

wait_player()
{
    cd bin

    player_num=9999
    while [ "$player_num" -ne "0" ]
    do
        server_info=`./server_info -n | tail -1`
        echo $server_info
        player_num=`echo $server_info | awk '{print $5}'`
        if [ -z $player_num ]
        then
            player_num=0
        fi
        #echo $player_num
        sleep 1
    done

    cd ..
    echo 'player num = 0'
}

wait_game()
{
    #echo wait_center
    grep_gameserver="no null"
    while [ -n "$grep_gameserver" ]
    do
        grep_gameserver=`find_pid_by_name gameserver`
        echo "waiting gameserver close"
        sleep 1
    done

    echo 'gameserver close'
}
wait_center()
{
    #echo wait_center
    grep_centerserver="no null"
    while [ -n "$grep_centerserver" ]
    do
        grep_centerserver=`find_pid_by_name centerserver`
        echo "waiting centerserver close"
        sleep 1
    done

    echo 'centerserver close'
}

stop()
{
    echo stop:
    # 删除运行标记文件
    if [ -f "../../sjsy_server_run" ]; then
        rm ../../sjsy_server_run > /dev/null
    fi
    # 先关闭所有gateserver 
    kill gateserver
    # 再等待所有玩家数据回存
    wait_player
    # 在关闭其他server
    kill flashserver
    kill timerserver
    kill gameserver
    # 等待gameserve安全退出
    wait_game
    kill centerserver
    # 等待centerserver关闭
    wait_center

    return
}

stop

. ./scan.sh
