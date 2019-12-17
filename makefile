make: client.c smtp.c pop.c
	rm -f client smtp pop
	gcc client.c -o client -lnsl
	gcc smtp.c -o smtp
	gcc pop.c -o pop
