make: client-f.c smtp-f.c pop-f.c ac-sv.c
	rm -f client smtp pop ac
	gcc client-f.c -o client -lnsl
	gcc smtp-f.c -o smtp
	gcc pop-f.c -o pop
	gcc ac-sv.c -o ac