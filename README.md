## A simple file server based on process pool.

### Process Pool Workflow:

- Workflow of Parent Process:

  1. Loop to create child processes, and parent process records related information of child processes (child process ID, child process status, file descriptors used for communication with child processes)

  2. Create TCP listening sockets and wait for client connections.

  3. Create epoll and add descriptors requiring monitoring to epoll.

  4. If a client connects to the server, accept function is used to receive this connection request, and newFd is returned, which is handed over to an idle child process.

  5. If the pipe monitored by epoll is readable, it means that the child process has completed its work, and the work status of the child process is set to idle by the parent process.

- Workflow of Child Process:

  1. The child process calls recvFd and blocks on the pipe. If there is data coming from the pipe, it returns from recvmsg and serves the client.

  2. After serving the client, close the connection, notify the parent process, and have the parent process set the work status of the child process to non-busy.

  3. Continue waiting for the next task.

### If a client wants to download a file from the server, how should the file be transmitted?

1. Sender:
   1. Open the file and send the file name.
   2. Read the file and send the file content.
2. Receiver:
   1. Receive the file and create a file with the same name.
   2. Receive the file content and write the received content to the file.

### some optimization

- zero copy
  - mmap
  - sendfile
  - splice
- process exit 
  - Send a signal to the parent process, and the parent process kills the child processes one by one after receiving the signal.
  - After receiving the signal, the parent process sends an exit signal to the child process. If the child process is busy, it waits for the work to finish before exiting; if the child process is idle, it exits directly.

