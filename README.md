Styx history synchronizer
=========================

# License
The MIT License (MIT)

Copyright (c) 2013 F. von Never

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

# Protocol
Server and client uses custom protocol based on protobuf. To generate protobuf
code for client and server, use `styx-messages/Build-Protobuf.ps1` file.

# Server
Styx server is written in Scala. It listens on 6060 port (default) for
incoming client connections.

## Running
To run the server, install `sbt` and use

    $ sbt run

# Client
For now, the only client is Miranda NG plugin.

## Building

### Compiler

Current client code is compatible only with MSVS IDE.

### Libraries

You'll need a boost logging library.

### Process

First of all, if you not plan to debug your code inside IDE, you're
free to disable the post-build action. Then, build projects as always.

## Using
Copy `*.dll` and `*.pdb` files to folder `Miranda/Plugins`. Start Miranda and
use main menu command `Enable Styx`.
