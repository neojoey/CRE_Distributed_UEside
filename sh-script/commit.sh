#!/bin/bash
NowDate=$(date +%Y%m%d)-$(date +%H%M)

git add ./scratch
git add ./src/lte/model
git commit -m $NowDate
git push
