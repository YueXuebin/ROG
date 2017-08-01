#

. ./utils.sh

kill_gs5()
{
    echo "kill gameserver 5"
    PIDS=`find_pid_by_name gameserver`
    #echo $PIDS
    
    for id in $PIDS
    do
        arg=`ps -Fp $id | tail -n 1 | awk '{print $13}'`
        #echo $arg
        if [ "$arg" = "5" ]
        then
            echo "kill gameserver 5 pid=$id"
            /bin/kill -s 2 $id
        fi
    done
}
kill_gs5

