package ru.org.codingteam.styx.server

import akka.actor._
import java.net.InetSocketAddress
import akka.actor.IO._
import akka.actor.IO.NewClient
import akka.actor.IO.ServerHandle
import akka.actor.IO.Closed
import akka.actor.IO.Listening

class ServerActor extends Actor with ActorLogging {

	var storage: ActorRef = null

	override def preStart() {
		storage = context.actorOf(Props[StorageActor])

		val address = new InetSocketAddress("localhost", 6060)
		IOManager(context.system).listen(address)
	}

	def receive = {
		case Listening(server, address) =>
			log.info("The server is listening on socket " + address)

		case NewClient(server) =>
			println("New incoming connection on server")
			val client = context.actorOf(ClientConnectionActor.props(storage))
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
