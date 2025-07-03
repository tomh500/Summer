#!/bin/bash

clear
cp ../Ticker_Gen_Ubuntu ./Ticker_Gen_Ubuntu
# 修复：赋予执行权限
chmod +x ./Ticker_Gen_Ubuntu
./Ticker_Gen_Ubuntu -f "GenData/High.txt"
rm ./Ticker_Gen_Ubuntu

