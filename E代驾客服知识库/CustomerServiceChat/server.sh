#!/bin/bash
######################################################
#uuencode需要安装sudo apt-get install sharutils
######################################################

mailto="robosay_chenpeng@126.com"

Global_MyName=""
Global_Watch="false"
Global_Port=""
Global_Program="./server"
Global_DataPath="./data"
Global_RootConf=$Global_DataPath"/root.conf"
Global_Upload="./upload"

SendCollapseMail(){
zhengwen="Hello $mailto,

Fail description: Robot service $Global_Program collapsed
Hostname            : $HOSTNAME
ip addr             : `ifconfig eth0 | awk '/inet/ {print $2}'| cut -f2 -d":"`
programPID          : $$ 

Regards,
Robosay robot team
-------------------------------------------------------------------------------
* This is an automatic mail from program [$Global_Program].
-------------------------------------------------------------------------------
"
(echo "$zhengwen";uuencode server.log server.log;uuencode in.xml in.xml;uuencode out.xml out.xml) | mail -s "[No-Reply]Fail Report from $Global_Program" $mailto
}
#uuencode 为邮件加附件使用的

CrashReport(){
	echo "sever crashed at `date +"%Y-%m-%d %H:%M:%S"`" >> report.log
}

# 监视服务
StartWatchServer(){
	CMD="$Global_Program $Global_RootConf $Global_Port"
	
	while [ 1 ]
	do
		PID=`ps -efwww | grep "$CMD" | grep -v "grep" | awk '{print $2}'`
		PID=`echo $PID | awk '{print $1}'`
		if [ -z "$PID" ]
		then
			CrashReport
			SendCollapseMail
			StartServer
		fi
		sleep 1
		MEM_USED=`ps aux|grep ./server|grep $PID|awk '{print $6}'|awk 'BEGIN{SUM0=0}{SUM0+=$1} END{print SUM0/1024}'`
		CPU_USED=`ps aux|grep ./server|grep $PID|awk '{print $3}'`
		TimeStamp=`date +"%y-%m-%d %H:%M:%S"`
		if [ "$Last_MEM_UESD" != "$MEM_USED" ]
		then
			echo "[$TimeStamp]	Mem used: $MEM_USED MB, CPU used: $CPU_USED%" >> server.log
		fi
#		echo "[$TimeStamp]	Mem used: $MEM_USED MB, CPU used: $CPU_USED%"
		sleep 1
		Last_MEM_UESD=`echo $MEM_USED`
	done
}

# 使用说明
UESD(){
	echo "#############################################################"
	echo "UESD"
	echo "./server.sh -p [ port-number ] -e [ start | stop | restart ] (-w)"
	echo "#############################################################"
}

# 等待服务启动完毕
WaitForStartFinish(){
	counter=0
	while [ 1 ]
	do
		if [ -f "server.$Global_Port.pid" ]
		then
			break
		else
			sleep 1
		fi
		counter=`expr $counter + 1`
		if [ "$counter" -eq "5" ]
		then
			echo "[$Global_MyName]服务启动超时"
			exit
		fi
	done
}

# 启动服务
StartServer(){
	`rm -f server.$Global_Port.pid`
	$Global_Program $Global_RootConf $Global_Port &
	echo "[$Global_MyName]启动服务"
	WaitForStartFinish
	PID=`cat server.$Global_Port.pid`
	echo "[$Global_MyName]启动服务完成，进程号为$PID"
}

# 尝试并启动服务
TryStartServer(){
	`rm -f server.$Global_Port.pid`
	if [ -f "server.$Global_Port.pid" ]
	then
		PID=`cat server.$Global_Port.pid`
		CMD="$Global_Program $Global_RootConf $Global_Port"
		PROCESSPID=`ps -efwww | grep "$CMD" | grep -v "grep" | awk '{print $2}'`
		MATCHED=`echo $PROCESSPID | grep PID`
		if [ "$MATCHED" != "" -a "$MATCHED" != "0"]
		then
			echo "[$Global_MyName]服务已经启动"
		else			
			rm -f server.$Global_Port.pid
			$Global_Program $Global_RootConf $Global_Port &
			echo "[$Global_MyName]启动服务"
			WaitForStartFinish
			PID=`cat server.$Global_Port.pid`
			echo "[$Global_MyName]启动服务完成，进程号为$PID"
		fi
	else
		$Global_Program $Global_RootConf $Global_Port &
		echo "[$Global_MyName]启动服务"
		WaitForStartFinish
		PID=`cat server.$Global_Port.pid`
		echo "[$Global_MyName]启动服务完成，进程号为$PID"
	fi
	
	# 启动监视进程
	if [ "$Global_Watch" = "true" ]
	then
		StartWatchServer
	fi
}

# 等待服务释放端口
WaitForPortUnbind(){
# 需要添加上限
	counter=0
	while [ "$counter" -le "5" ]
	do
		LINTENING=`netstat -ant | grep :$Global_Port | awk '{print $6}'`
		if [ "$LINTENING" = "" ]
		then
			sleep 1
			break
		else
			sleep 1
		fi
		counter=`expr $counter + 1`
	done
	
}

# 关闭服务
StopServer(){
	CMD="$Global_Program $Global_RootConf $Global_Port"
	PID=`ps -efwww | grep "$CMD" | grep -v "grep" | awk '{print $2}'`
	if [ "$PID" != "" ]
	then
		kill $PID
		rm -f server.$Global_Port.pid
		echo "[$Global_MyName]关闭服务，进程号为$PID"
		WaitForPortUnbind
	else
		echo "[$Global_MyName]系统中无服务"
	fi
}

main(){
	SHELL_PATH=$(cd "$(dirname "$0")";pwd)
	echo $SHELL_PATH
	cd $SHELL_PATH
	
	Global_MyName=$0
	until [ $# -eq 0 ]
		do
		case $1 in
			-p)
			Global_Port=$2;
			shift;shift;;
			-e)
			STATUS=$2;
			shift;shift;; #shift会把参数列表之前的参数移除掉
			-n)
			Global_Program=$Global_Program_$2;
			shift;shift;;
			-w)
			Global_Watch="true";
			shift;
		esac
	done
	
	if [ "$Global_Port" != "" ]
	then
		if [ "$STATUS" != "" ]
		then
			case "$STATUS" in
				start)
					TryStartServer
					;;
				stop)
					StopServer
					;;
				restart)
					StopServer
					TryStartServer
					;;
			esac
		else
			UESD
		fi
	else
		UESD
	fi
}
###########################################################################
main $@

