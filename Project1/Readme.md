# CSIE 3510: Computer Networks PJ#1

## <strong>Execution</strong>
- Platform: `linux (CSIE workstation)`
- Submitted File: `server.c`, `client.c`, `Makefile`

1. Change the permission of all the file so that you can run it `chmod +x [filename]`.<br><br>
2. Type the command `make` to make the execution file of server.
3. If the command above doesn't work, try the command `gcc server.c -o server; gcc client.c -o client`.<br><br>
4. Type the command `./server [listen_port]` and `./client [-n] [-t] [host:port]` to test if the server and client meet all the reuirements.

## <strong>Self Examination</strong>
1. Client can receive and send message to single server with IP/Port.
2. Client can translate host name into IP and receive and send message to server.
3. Client can receive and send message to multiple server.<br><br>
4. My program supports `-n -t` command.
5. Server can receive and send message from one client.
6. Server can receive and send message from multiple client.<br><br>
7. Bonus: Client can ping multiple servers simultaneously.