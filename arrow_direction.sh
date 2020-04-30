#!/bin/sh

if [ $# -ne 5 ]; then
    echo 'Wrong number of arguments, please use the script like this: '
    echo './arrow_direction.sh arrow_direction region_url workspace_id device_id access_token'
else
    arrow_direction=$1
    region_url=$2
    workspace_id=$3
    device_id=$4
    access_token=$5
    echo 
    curl -s -H "Content-Type: application/json" \
       -H "Authorization: Bearer ${access_token}"\
       -X POST ${region_url}/workspaces/${workspace_id}/devices/${device_id}/functions/arrow_direction_function \
       -d "{ \
              \"parameters\": { \
                \"arrow_direction\": \"${arrow_direction}\" \
              }\
            }"
fi
