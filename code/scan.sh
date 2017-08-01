
. ./utils.sh

search()
{
    PIDS=`find_pid_by_name $1`

    echo $1 list:
    for id in $PIDS
    do
        # 显示
        ps -Fp $id | tail -n 1
    done
}

scan()
{
    echo scan:
    echo `pwd`
    search centerserver
    search gateserver
    search gameserver
}
scan

scan_monitor()
{
    if [ -f "../../pymonitor/sjsy-monitor.pid" ]; then
        echo "sjsy-monitor: start"
    else
        echo "sjsy-monitor: stop"
    fi
}
scan_monitor

