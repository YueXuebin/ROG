
. ./utils.sh

CENTER_PIDS=`find_pid_by_name centerserver`
GAME_PIDS=`find_pid_by_name gameserver`
GATE_PIDS=`find_pid_by_name gateserver`

PIDS=$CENTER_PIDS
for id in $GAME_PIDS
do
    PIDS=$PIDS,$id
done

for id in $GATE_PIDS
do
    PIDS=$PIDS,$id
done

for id in $FLASH_PIDS
do
    PIDS=$PIDS,$id
done

for id in $TIME_PIDS
do
    PIDS=$PIDS,$id
done

top -p $PIDS
