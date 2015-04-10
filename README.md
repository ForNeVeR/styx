Styx
====

[![Pull request statistics](http://issuestats.com/github/ForNeVeR/styx/badge/pr?style=flat-square)](http://www.issuestats.com/github/ForNeVeR/styx)
[![Issue statistics](http://issuestats.com/github/ForNeVeR/styx/badge/issue?style=flat-square)](http://www.issuestats.com/github/ForNeVeR/styx)
[![Build status](https://travis-ci.org/ForNeVeR/styx.png?branch=develop)](https://travis-ci.org/ForNeVeR/styx)

Project status: development.

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
