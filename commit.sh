#!/bin/bash
NowDate=$(date +%Y%m%d)-$(date +%H%M)

find ./* -size +50M |sed 's|^\./||g' | cat > .gitignore

git add ./scratch
git add ./src/lte/model
git add ./Results*
git add ./*.sh
git add ./sh-script
git commit -am $NowDate
git push 
