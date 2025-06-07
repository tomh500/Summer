@echo off
cls
copy ..\Ticker_gen.exe Ticker_gen.exe 
Ticker_gen.exe -f "GenData\High.txt"
del Ticker_gen.exe 
