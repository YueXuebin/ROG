#

. ./utils.sh

kill_gs6()
{
    echo "kill gameserver 6"
    PIDS=`find_pid_by_name gameserver`
    #echo $PIDS
    
    for id in $PIDS
    do
        arg=`ps -Fp $id | tail -n 1 | awk '{print $13}'`
        #echo $arg
        if [ "$arg" = "6" ]
        then
            echo "kill gameserver 6 pid=$id"
            /bin/kill -s 2 $id
        fi
    done
}
kill_gs6

