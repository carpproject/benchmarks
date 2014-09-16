#!/bin/bash

set -e
set -o pipefail
./spmv-sp ellpackr | grep TOTAL_TIME | sed -e 's/TOTAL_TIME: //'
