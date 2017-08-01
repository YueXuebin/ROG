#
# 工具函数

# 说明:根据进程pid,查找进程全路径名
# 参数1:pid
# 返回:进程全路径
find_name_by_pid()
{
    str=`ls -al /proc/$1/exe | awk '{print $11}'`
    echo $str
}

# 说明:查找源路径是否包含子路径
# 参数1:源路径
# 参数2:子路径
# 返回: 1.包含子路径 0.不包含子路径
is_has_path()
{
    str1=$1
    str2=$2
    replace=${str1#*$str2}

    if [ "$str1" = "$replace" ]
    then
        echo 0
    else
        echo 1
    fi
}

# 说明:查找进程pid的执行文件是否在当前目录下
# 参数1:pid
# 返回: 1.在当前目录下 0.不在当前目录下
is_in_path()
{
    PNAME=`find_name_by_pid $1`
    CUR_DIR=`pwd`
    is_has_path $PNAME $CUR_DIR
}

# 说明:根据进程名,查找当前目录下的进程id
# 参数1:进程名 
find_pid_by_name()
{
    PID=`ps -e | grep $1 | grep -v "grep" | awk '{print $1}'`
    for id in $PID
    do
        POK=`is_in_path $id`
        if [ "$POK" -eq "1" ]
        then
            echo $id
        fi
    done
}
