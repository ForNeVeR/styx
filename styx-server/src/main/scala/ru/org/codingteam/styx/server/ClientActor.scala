package ru.org.codingteam.styx.server

import akka.actor.{Actor, ActorLogging}
import akka.util.ByteString
import akka.actor.IO.{SocketHandle, Closed, Read}
import ru.org.codingteam.styx.Message.Ping
import com.google.protobuf.{InvalidProtocolBufferException, CodedInputStream}

class ClientActor extends Actor with ActorLogging {

	var buffer = ByteString()

	def receive = {
		case Read(socket, bytes) =>
			log.info("Received incoming data from socket")
			receiveBytes(bytes)

		case Closed(socket: SocketHandle, cause) =>
			log.info("Socket has closed, cause: " + cause)
			context.stop(self)
	}

	def receiveBytes(bytes: ByteString) {
		buffer = buffer ++ bytes
		val length = buffer.length
		log.info(s"Buffer length = $length bytes")

		tryDeserialize()
	}

	def tryDeserialize() {
		val stream = CodedInputStream.newInstance(buffer.toArray)
		try {
			val result = Ping.parseFrom(stream)
			val message = result.getMessage
			log.info(s"Received ping message: $message")

			buffer = buffer.drop(stream.getTotalBytesRead)
			val length = buffer.length
			log.info(s"Remaining $length bytes")
		} catch {
			case error: InvalidProtocolBufferException =>
				log.error(error, "Possible unfinished message")
		}
	}
}
