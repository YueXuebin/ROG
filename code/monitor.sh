#

. ./utils.sh

username=`users | awk '{print $1}'`

search_ret=0    # 是否找到指定的进程
search()
{
    #echo $1
    #echo $2
    PIDS=`find_pid_by_name $1`
    #echo $PIDS

    search_ret=0
    for id in $PIDS
    do
        STR=`ps -ef | grep $id | grep -v "grep"`
        #echo $STR
        REP_STR=${STR#*$1 $2}
        #echo $REP_STR

        if [ "$STR" != "$REP_STR" ]
        then
           search_ret=1
        fi
    done
}

restart()
{
    # 重启$1
    #sleep 5
    cd bin
    ulimit -c unlimited
    nohup ./$1 $2 > /dev/null 2>&1 &
    cd ..
}

check()
{
    #echo $1
    # 检查$1
    search "$1" "$2"
    #echo $search_ret
    if [ $search_ret -eq 0 ]
    then
        restart "$1" "$2"
    fi
}

check_all()
{
    #pwd

    check "gameserver" "-i 1"
    check "gameserver" "-i 2"
    check "gameserver" "-i 3"
    check "gameserver" "-i 4"
    check "gateserver" "-i 1"
    check "gateserver" "-i 2"
}
check_all

