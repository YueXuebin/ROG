#

s=2

if [ -z $1 ]
then
    echo no cmd
    exit
fi


while [ true ]
do
    $1
    sleep $s
done
