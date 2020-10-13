rm *o
rm *exe
gcc -g -c timerlib.c -o timerlib.o
gcc -g -c timerlib_test.c -o timerlib_test.o
gcc -g timerlib.o timerlib_test.o -o exe -lrt
