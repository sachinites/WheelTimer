rm *o
rm *exe
gcc -g -c rt.c -o rt.o
gcc -g -c rt_entry_expiration.c -o rt_entry_expiration.o
gcc -g rt_entry_expiration.o rt.o -o rt_entry_expiration.exe
