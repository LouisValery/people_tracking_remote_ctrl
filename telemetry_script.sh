#!/bin/sh

if [ $# -ne 4 ]; then
    echo 'Wrong number of arguments, please use the script like this: '
    echo './arrow_direction.sh arrow_direction region_url workspace_id device_id access_token'
else
    telemetry_label=$1
    region_url=$2
    workspace_id=$3
    access_token=$4
    echo 
    curl -s -H "Content-Type: application/json" \
       -H "Authorization: Bearer ${access_token}"\
       -X GET "${region_url}/workspaces/${workspace_id}/telemetry?labels=${telemetry_label}&sort=timestamp&sort_direction=DESC&limit=1"
fi



