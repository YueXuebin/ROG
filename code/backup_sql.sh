

filename=`date +%F-%H-%M-%S`
echo $filename
mysqldump -uroot -pkunlun123sjsy456 --databases sjsy_db > sjsy_db_$filename.sql
