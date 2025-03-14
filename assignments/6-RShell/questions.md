1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

   The remote client knows it has received the full output of a command when it detects a predefined **end-of-message delimiter**. In this case, the **EOF character (`0x04`)** is used to indicate that the server has finished sending data.

   Techniques to handle partial reads and ensure complete message transmission:
   - **Repeated `recv()` calls**: Since TCP does not preserve message boundaries, the client must continuously call `recv()` until it detects the EOF character.
   - **Buffering received data**: The client can store incoming bytes in a buffer and process them once the entire message is received.
   - **Length-prefixed protocol**: The server can first send a fixed-size header specifying the length of the message, allowing the client to read exactly that many bytes.

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

   A networked shell protocol must explicitly define message boundaries because TCP treats data as a continuous stream. This can be handled using:
   - **Delimiters**: Special characters like `'\0'` (null terminator) for command separation and `0x04` (EOF) to mark the end of output.
   - **Length-prefixing**: Prepending a fixed-size header that specifies the message length before sending data.

   Challenges if boundaries aren’t handled correctly:
   - **Command fragmentation**: A single command may be split across multiple packets, causing incomplete execution.
   - **Command merging**: Two commands could be received together, leading to incorrect parsing and execution.
   - **Deadlocks or hangs**: If a receiver fails to recognize message boundaries, it may wait indefinitely for more data.

3. Describe the general differences between stateful and stateless protocols.

   - **Stateful protocols**: Maintain session information between requests, allowing for continuous interactions. Example: **TCP, FTP, SSH**.
   - **Stateless protocols**: Treat each request independently, requiring no stored session state. Example: **HTTP, UDP, DNS**.

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

   Despite being unreliable (no guaranteed delivery, ordering, or error correction), **UDP** is useful for:
   - **Low-latency applications**: Real-time gaming, VoIP, and video streaming prioritize speed over reliability.
   - **Broadcast & multicast support**: Unlike TCP, UDP enables communication with multiple clients simultaneously.
   - **Lightweight and connectionless communication**: No handshake or connection state, making it ideal for services like **DNS and DHCP**.

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

   The **Berkeley Sockets API** (or Winsock on Windows) provides an interface for network communication. Key functions include:
   - **`socket()`** – Creates a network socket.
   - **`bind()`** – Assigns an IP address and port to the socket.
   - **`listen()`** – Puts the socket in passive mode to accept connections.
   - **`accept()`** – Accepts incoming client connections.
   - **`connect()`** – Establishes a connection to a remote server.
   - **`send()` / `recv()`** – Sends and receives data over a socket.
   - **`close()`** – Closes the socket.

   This API allows applications to communicate over **TCP/IP or UDP** efficiently.

