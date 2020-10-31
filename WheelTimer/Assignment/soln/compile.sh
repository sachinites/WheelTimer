rm *o
rm *exe
gcc -g -c rt.c -o rt.o
gcc -g -c rt_entry_expiration.c -o rt_entry_expiration.o
# below two lines compiles the WheelTimer Library
gcc -g -c ../../WheelTimer.c -o ../../WheelTimer.o
gcc -g -c ../../gluethread/glthread.c -o ../../gluethread/glthread.o
gcc -g rt_entry_expiration.o rt.o ../../WheelTimer.o ../../gluethread/glthread.o -o rt_entry_expiration.exe -lpthread
