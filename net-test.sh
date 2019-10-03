#!/bin/bash


delay=600;
noConnectionMessage="No connection, trying again in $delay seconds...";
ConnectionMessage="We have Internet, trying again in $delay seconds...";


echo "INTERNET TESTER: Type Ctrl+C to quit";
rm --force /tmp/index.site

while [ 1 -eq 1 ]; do
   if [[ $(nmcli -f STATE -t g) != 'connected' ]]; then
       echo $noConnectionMessage; 
	service network-manager restart
       sleep 10;
   #else 
       #echo $ConnectionMessage;
   fi
   sleep $delay;
done

exit 0;

