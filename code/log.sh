logfile=`ls -l --sort t log/ | head -2 | tail -1 | awk '{print $9}'`
echo $logfile
cat log/$logfile
