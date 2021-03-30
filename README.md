
## Descrição

O trabalho consiste em um serviço de envio de notificações estilo “feed de notícias”, muito similar ao Twitter. O projeto foi implementado em C/C++, utilizando a API Transmission Control Protocol (TCP) sockets do Unix. 

## Getting Started

### Dependencias

* Ambientes Unix (Linux)
* GCC 9.3.0
* lib pthread

### Compilação

* Para compilar o projeto é necessário apenas rodar o comando `make` que irá compilar todos os arquivos `.c` e suas dependências.

````
make
````
````
make clean
````

### Executando o programa

* Para rodar o servidor é preciso apenas rodar o seguinte comando:
```
./server
```

* Para rodar as instâncias de cliente é preciso um nome de usuário, rodar no IP `127.0.0.1` e porta `2000`, segue o exemplo abaixo:

````
./client @pedro 127.0.0.1 2000
````

## Autores

* Leandro de Oliveira Pereira - 00273114
* Pedro Ceriotti Trindade - 00264846
* Mauricio Holler Guntzel - 00273108
* Alexandre Santos da Silva Jr. - 00193093
