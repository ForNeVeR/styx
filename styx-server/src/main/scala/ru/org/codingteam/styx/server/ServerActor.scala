package ru.org.codingteam.styx.server

import akka.actor.{Props, IOManager, ActorLogging, Actor}
import java.net.InetSocketAddress
import akka.actor.IO._

class ServerActor extends Actor with ActorLogging {

	override def preStart() {
		val address = new InetSocketAddress("localhost", 6060)
		IOManager(context.system).listen(address)
	}

	def receive = {
		case Listening(server, address) =>
			log.info("The server is listening on socket " + address)

		case NewClient(server) =>
			println("New incoming connection on server")
			val client = context.actorOf(Props[ClientConnectionActor])
			try {
				server.accept()(client)
			} catch {
				case error: Throwable =>
					log.error(error, "Connection error")
					context.stop(client)
			}

		case Closed(server: ServerHandle, cause) =>
			log.info("Server socket has closed, cause: " + cause)
	}
}
