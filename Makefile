default: server teste

mensagem.o: src/mensagem.c
	gcc -g -c src/mensagem.c -o obj/mensagem.o -I headers/

estruturas.o: src/estruturaListas.c
	gcc -g  -c src/estruturaListas.c -o obj/estruturas.o -I headers/

server.o: estruturas.o mensagem.o src/servidor.c
	gcc -g  -c src/servidor.c -o obj/server.o -I headers/

teste.o: mensagem.o src/teste.c 
	gcc -g  -c src/teste.c -o obj/teste.o -I headers/

server: server.o
	gcc -g obj/server.o obj/mensagem.o obj/estruturas.o -o server -lpthread	

teste: mensagem.o teste.o 
	gcc -g obj/teste.o obj/mensagem.o -o teste -lpthread

clean:
	rm -f server teste obj/*.o
