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
All client messages are sorted by UTC date. Then they are chunked with the rolling hash algorithm. Then for each chunk the following algorithm should be performed:

    for each Chunk on client
    	client_send(ChunkHash)
    	server_send(ChunkHashResult)
    	if (ChunkHashResult is negative)
    		for each Message in Chunk on client
    			client_send(Message)
    			server_send(MessageResult)
    		client_send(ChunkHash)
    		server_send(ChunkHashResult)
    		if (ChunkHashResult is negative)
    			for each Message in Chunk on server
    				client_send(Message)
    				server_send(MessageResult)

If a message is received by client while hashing stage is performed, and this message were put inso one of the already synced chunks, then the client should restart the hashing from the changed chunk.

If a message is received by the server while hashing stage is performed, server should notify the client about that by the flags inside the next MessageResult packet. Client then should restart the hashing from the changed chunk.

# Messaging
This stage repeats continously after the hashing stage. Any participant (client or server) can initiate the sequence.

1. client: `Message`
2. server: `MessageResult`

Or:

1. server: `Message`
2. client: `MessageResult`
