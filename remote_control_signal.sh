#!/bin/sh

if [ $# -ne 5 ]; then
    echo 'Wrong number of arguments, please use the script like this: '
    echo './remote_control_signal.sh allow_remote_control region_url workspace_id device_id access_token'
else
    remote_control_signal=$1
    region_url=$2
    workspace_id=$3
    device_id=$4
    access_token=$5
    echo 
    curl -s -H "Content-Type: application/json" \
       -H "Authorization: Bearer ${access_token}"\
       -X POST ${region_url}/workspaces/${workspace_id}/devices/${device_id}/functions/remote_control_signal_function \
       -d "{ \
              \"parameters\": { \
                \"remote_control_signal\": ${remote_control_signal} \
              }\
            }"
fi
