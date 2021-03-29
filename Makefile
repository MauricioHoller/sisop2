default: server client

mensagem.o: src/mensagem.c
	gcc -g -c src/mensagem.c -o obj/mensagem.o -I headers/

estruturas.o: src/estruturaListas.c
	gcc -g -c src/estruturaListas.c -o obj/estruturas.o -I headers/

server.o: estruturas.o mensagem.o src/servidor.c
	gcc -g -c src/servidor.c -o obj/server.o -I headers/

client.o: mensagem.o src/cliente.c 
	gcc -g -c src/cliente.c -o obj/client.o -I headers/

server: server.o
	gcc -g obj/server.o obj/mensagem.o obj/estruturas.o -o server -lpthread	

client: mensagem.o client.o 
	gcc -g obj/client.o obj/mensagem.o -o client -lpthread

clean:
	rm -f server client obj/*.o
