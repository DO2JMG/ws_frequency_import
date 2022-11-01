#!/bin/bash
#
#
#
# Frequency import for dxlAPRS
# Copyright (C) Jean-Michael Grobel (DO2JMG) <info@wettersonde.net>
#
# v0.1

workingdir="$( cd "$(dirname "$0")" ; pwd -P )"
source ${workingdir}/ws-import.conf
piddir=${workingdir}/pidfiles
bindir=${workingdir}/bin

beaconfile=${workingdir}/beacon.txt
commentfile=${workingdir}/comment.txt

FQIMPORT=${bindir}/fqimport

command -v ${FQIMPORT} >/dev/null 2>&1 || { echo "Ich vermisse " ${FQIMPORT} >&2; exit 1; }


function startfqimport {
  echo "Starte Frequenzimport"
  ${FQIMPORT} -r ${range} -f ${channelfile} -lat ${latitude} -lon ${longitude} -sql ${squelch} 2>&1 >> ${LOGFILE} & 
  fqimport_pid=$!
  echo $fqimport_pid > $PIDFILE
}


function sanitycheck {
# check pidfiles in piddir
  shopt -s nullglob # no error if no PIDFILE
  for f in ${piddir}/*.pid; do
  pid=`cat $f`
    if [ -f /proc/$pid/exe ]; then ## pid is running?
      echo "$(basename $f) ok pid: $pid"
    else ## pid not running
      echo "$(basename $f) died"
      rm $f
    fi
  done
}


function checkproc {
#checks if prog is running or not
  if [ -s $PIDFILE ];then ##have PIDFILE
    pid=`cat $PIDFILE`
    if [ -f /proc/$pid/exe ]; then ## pid is running?
      return 0
    else ## pid not running
      return 1
    fi
  else ## no PIDFILE
    return 1
  fi
}


tnow=`date "+%x_%X"`
echo $tnow

### kill procs
if [ "x$1" == "xstop" ];then
  killall fqimport
  exit 0
fi


# ## check for fqimport
LOGFILE=/tmp/fqimport.log
PIDFILE=${piddir}/fqimport.pid

 checkproc
 returnval=$?
 if [ $returnval -eq 1 ];then
   : > ${LOGFILE}
   startfqimport
 fi


exit 0

