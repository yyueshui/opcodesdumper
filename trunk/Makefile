CC = gcc
CFLAGS = 	-I/usr/local/include/php/ \
            -I/usr/local/include/php/main \
            -I/usr/local/include/php/Zend \
            -I/usr/local/include/php/TSRM 
LDFLAGS = -lstdc++ -L/usr/local/lib -lphp5
ALL:
	$(CC) -o opcodes_dumper opcodes_dumper.cpp $(CFLAGS) $(LDFLAGS)
