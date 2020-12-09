# Network-server-client

# How to run the code:

1) Place the server and client code in any directories you need and open terminal with their respective address. 

2) Then complie respective codes in their current directory with gcc -o.

    > gcc -o server server.c

    This will create new exectable file "server" in it's current directory.

    > gcc -o client client.c

    Also this will again create a new exectable "client".

3) Now, First run the "server" exectable file by running "./server" in the terminal.

    > ./server

4) Then now run "client" file by running "./client".

    > ./client

5) Then you can see terminal run by client program like this.

    > client>

## Commands that can be exectuted:

> client>​ __get__  _fileName_

        Use: The file if exists at the server side will be downloaded in the client directory else gives appropriate error.

> client> ​ __get__ _file1_  _file2_  _file3_

    Use: Multiple files requested will be downloaded in the client directory.


> client> ​ __exit__

    Use: connection with the server is closed.
