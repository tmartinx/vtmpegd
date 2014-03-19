all: 
	make all -C src/server
	make all -C src/client

clean: 
	make clean -C src/server 
	make clean -C src/client
