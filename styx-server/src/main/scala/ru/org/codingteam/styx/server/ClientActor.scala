package ru.org.codingteam.styx.server

import akka.actor.{Actor, ActorLogging}
import akka.util.ByteString
import akka.actor.IO.{SocketHandle, Closed, Read}
import com.google.protobuf.{InvalidProtocolBufferException, CodedInputStream}
import ru.org.codingteam.styx.Datagram.Message

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
			val result = Message.parseFrom(stream)
			log.info(s"Received message: $result")

			buffer = buffer.drop(stream.getTotalBytesRead)
			val length = buffer.length
			log.info(s"Remaining $length bytes")
		} catch {
			case error: InvalidProtocolBufferException =>
				log.error(error, "Possible unfinished message")
		}
	}
}
