#!/bin/bash

diff hcc2 hcc3
if [ $? -eq 0 ]; then
    echo "OK"
else
    echo "hcc2 and hcc3 aren't the same."
    exit 1
fi