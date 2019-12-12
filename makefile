make: client.c smtp.c pop.c
	rm -f cliant smtp pop
	gcc cliant.c -o cliant -lnsl
	gcc smtp.c -o smtp
	gcc pop.c -o pop
