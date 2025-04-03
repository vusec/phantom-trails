#!/bin/bash
# Simple helper to just fuzz a specific config.

boom_config="${1}"
if [ -z "$boom_config" ]
then
      echo "No BOOM config set, assuming FuzzConfig (MediumBoom) by default"
      boom_config="FuzzConfig"
fi

python3 /scripts/phantom-trails fuzz --config=$boom_config || echo "Killed at $(date)"
