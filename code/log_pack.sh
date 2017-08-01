#
#
# log文件打包

timetag=`date +%Y_%m_%d_%H_%M_%S`
echo $timetag

LOG_DIR="bin/log/"

if [ -d $LOG_DIR ] 
then
    tar -zcvf $LOG_DIR/log_$timetag.tar.gz $LOG_DIR/*.log
    rm -f $LOG_DIR/*.log
fi
