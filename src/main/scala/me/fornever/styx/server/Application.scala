package me.fornever.styx.server

import akka.actor.{Props, ActorSystem}

object Application extends App {
	val system = ActorSystem()
	val server = system.actorOf(Props[ServerActor])
}
