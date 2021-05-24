#!/bin/bash
printf '%d' $(sensors | grep 'id 0:' | awk '{ print $4 }') 2>/dev/null
