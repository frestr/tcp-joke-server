# TCP joke server
A simple TCP server that will serve you a joke.

## Compiling
Requirements: Linux, g++, make

Clone repo, go to project directory and run
```
make
```
to compile.

## Running
In one terminal, start the server:
```
$ ./server 4242
Listening on localhost:4242 ...
```

And in another, connect to the server:
```
$ nc localhost 4242
What's the worst thing about TCP jokes?
I'll keep telling it slower and slower until you get it.
```
