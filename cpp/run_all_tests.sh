#!/bin/bash

set -e
set -x

# Run the `basic_server_for_test` first for `test_basic_call`
nohup ./basic_server_for_test &
sleep 3

for file in `ls`; do
    if [ "test_" = "${file:0:5}" ];then
        echo "===========================RUNNING test $file==============================="
        ("./$file")
        echo "===========================PASSED  test $file==============================="
        echo ""
        echo ""
        echo ""
    fi
done

# Kill the `basic_server_for_test``
# ps -ef |grep basic_server_for_test |awk '{print $2}'|xargs kill -9
