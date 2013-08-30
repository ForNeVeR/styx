package ru.org.codingteam.styx.server

abstract sealed class SynchronizerState
case object NotConnected extends SynchronizerState
case object Handshake extends SynchronizerState
case object Hashing extends SynchronizerState
case object Messaging extends SynchronizerState
