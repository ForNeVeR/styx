Styx protocol definition
========================

# Packet structure
Any Styx network packet can be represented by the following structure:

    length : 4 bytes
    type   : 4 bytes
    body   : <length> bytes

Packet body is encoded by the protobuf library.

# Session stages
1. Handshake
2. Hashing
3. Messaging

# Handshake
1. TCP connection.
2. client: `Login`
3. server: `LoginResult`

# Hashing
On this stage client and server should check data consistency.

1. client: `Hash`
2. server: `HashResult`

If `HashResult` is negative, client should break current hashed region into smaller chunks and repeat the procedure. Finally, chunk is of size 1...

TODO: Desing this part.

# Messaging
This stage repeats continously after the hashing stage. Any participant (client or server) can initiate the sequence.

1. client: `Message`
2. server: `MessageResult`

Or:

1. server: `Message`
2. client: `MessageResult`
