#CORE_FILE=`ls /home/core -l --sort t | head -2 | tail -1 | awk '{print $9}'`
CORE_FILE=`ls bin/core* -l --sort t | awk '{print $9}'`
CORE_FILE1=`echo $CORE_FILE | awk '{print $1}'`
echo $CORE_FILE1
sudo chmod 777 $CORE_FILE

gdb -q ./bin/gameserver $CORE_FILE
#gdb -q ./bin/centerserver $CORE_FILE
#gdb -q ./bin/gateserver $CORE_FILE
#gdb -q ./bin/timerserver $CORE_FILE
#gdb -q ./bin/server_info $CORE_FILE
