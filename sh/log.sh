#!/usr/bin/bash
#******************************************************************************
#
# * File: nc.sh
#
# * Author:  Umut Sevdi
# * Created: 05/10/23
# * Description: A shell-script for obtaining required log information
#*****************************************************************************

function Help {
   echo "loglist - A helpful script that prints out formatted logs of given"
   echo "application"
   echo "Syntax: [-h/c|m]"
   echo
   echo "Options:"
   echo "-h/--help            Prints this menu."
   echo "-n/--nextcloud       Display logs of Nextcloud."
   echo "-s/--site            Display logs of the website."
   echo
}

function nextcloud {
    docker logs app_nc_1  -n 100 2>/dev/null | \
        awk '{printf "%s %s %s\t%s\t%s\n", $1,$9, $3,$12, $7}' | \
        grep -E "[0-9\. ]+ [A-Za-z]+"
}

#******************************************************************************
#                               www.service
#******************************************************************************

function site {
    journalctl -u www.service --no-pager | grep main | \
        awk '{printf "[%s %s] %s %s %s\t%s\t%s\n",$6,$7,$8,$12,$13,$14,$15}'
}

# returns ip addresses of recent clients
function ip {
    journalctl -o cat -u www.service --no-pager | awk '{print $7}' | \
        grep "[0-9.]" | uniq | tail
}

function pages {
    journalctl -o cat -u www.service --no-pager | grep "\[main" | \
        awk '{printf "%s %s %.30s %s\n",$7, $8, $9,$10}' | sed "s/serveStatic# /\//g" | \
        sed "s/serve#/\//g" | tr -d "]" | sed -E "s/ ([0-9]{3})/\t\1/g" | \
        sed -E "s/\/ ([A-Za-z]*)/\/\1/g" | \
        sed -E "s/([0-9.]+) ([^\0]+)\t([0-9]+)/\1\t\3\t\2/g" | uniq | tail
}


for arg in $@; do
    if [ $arg = -h ] || [ $arg = --help ]; then
        Help
        exit
    elif [ $arg = -n ] || [ $arg = --nextcloud ]; then
        nextcloud
        exit
    elif [ $arg = -s ] || [ $arg = --site ]; then
        pages
        exit
    else
        echo -e "Error: Invalid arguments" 1>&2
        exit
    fi
done
Help
