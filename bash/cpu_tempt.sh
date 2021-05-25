#!/bin/bash
# printf '%d' $(sensors | grep 'id 0:' | awk '{ print $4 }') 2>/dev/null

cpu=$(</sys/class/thermal/thermal_zone0/temp)
echo "$((cpu/1000))"
