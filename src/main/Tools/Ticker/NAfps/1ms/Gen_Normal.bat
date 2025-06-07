@echo off
cls
copy ..\Ticker_gen.exe Ticker_gen.exe 
Ticker_gen.exe -f "GenData\Normal.txt"
del Ticker_gen.exe 