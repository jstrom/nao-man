
LOG_SOURCE=/tmp/joints_log.xls
LOG_BACKUP=~/Desktop/master_joints_log.xls


if [ "$1" == "" ]; then
    echo "$> ./unitTest.sh [backup | compare | clear]"
    echo "   Option 'backup' makes a backup"
    echo "   Option 'compare' compares the current joints file to the backup"
    echo "   Option 'clear' deletes the source and backup"
    exit
fi;

if [ "$1" == "backup" ]; then
    echo "cp $LOG_SOURCE $LOG_BACKUP"
    cp $LOG_SOURCE $LOG_BACKUP
fi;

if [ "$1" == "compare" ]; then
    echo "diff $LOG_SOURCE $LOG_BACKUP"
    diff $LOG_SOURCE $LOG_BACKUP
fi;

if [ "$1" == "clean" ]; then
    echo "rm $LOG_SOURCE $LOG_BACKUP"
    rm $LOG_SOURCE $LOG_BACKUP
fi;


