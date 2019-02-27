#!/bin/bash

echo "# snapnum       a         z"
ls -1 $1 | grep AHF | sed 's/_/ /g' | sed 's/\.z/ /g' | \
        sed 's/\.AHF halos/ /g' | sed 's/sussing//g' | sed 's/\.z/ /g' | \
        awk '{printf "%8s    %.6f %8s\n", $1, 1./(1.+$2), $2}'

