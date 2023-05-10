#!bin/bash
#******************************************************************************
#
# * File: nc.sh
#
# * Author:  Umut Sevdi
# * Created: 05/10/23
# * Description: A shell-script for obtaining required log information
#*****************************************************************************

function Help {
   echo "loglist"
   echo "  A helpful script that prints out formatted logs of given application"
   echo
   echo "Syntax: [-h/c|m]"
   echo
   echo "Options:"
   echo "-h/--help            Prints this menu."
   echo "-n/--nextcloud       Display logs of Nextcloud."
   echo "-s/--site            Display logs of the website."
   echo
}

function nextcloud {
    docker logs app_nc_1  | grep GET | \
        awk '{printf "%s%s - %s %s %s::%s\n", $4,$5, $1, $3, $12, $7}'
}

function site {
    journalctl -u www.service --no-pager | grep main | \
        awk '{printf "[%s %s] %s %s %s\t%s\t%s\n",$6,$7,$8,$12,$13,$14,$15}'
}


for arg in $@; do
    if [ $arg = -h ] || [ $arg = --help ]; then
        Help
        exit
    elif [ $arg = -n ] || [ $arg = --nextcloud ]; then
        nextcloud
        exit
    elif [ $arg = -s ] || [ $arg = --site ]; then
        site
        exit
    else
        echo -e "Error: Invalid arguments" 1>&2
        exit
    fi
done
Help
