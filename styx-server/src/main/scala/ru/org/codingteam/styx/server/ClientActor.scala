package ru.org.codingteam.styx.server

import akka.actor.{Actor, ActorLogging}
import akka.util.ByteString
import akka.actor.IO.{SocketHandle, Closed, Read}
import com.google.protobuf.{InvalidProtocolBufferException, CodedInputStream}
import ru.org.codingteam.styx.Datagram.Message
import java.nio.ByteBuffer

class ClientActor extends Actor with ActorLogging {

	def receive = {
		case Read(socket, bytes) =>
			log.info("Received incoming data from socket")
			receiveBytes(bytes)

		case Closed(socket: SocketHandle, cause) =>
			log.info("Socket has closed, cause: " + cause)
			context.stop(self)
	}

	private var buffer = ByteString()

	private def dataLength = ByteBuffer.wrap(buffer.take(4).toArray).getInt

	private def receiveBytes(bytes: ByteString) {
		buffer = buffer ++ bytes
		val length = buffer.length
		log.info(s"Buffer length = $length bytes")
		if (length >= 4) {
			log.info(s"Data length = $dataLength bytes")
		    if (length >= dataLength + 4) {
			    tryDeserialize()
		    }
		}
	}

	private def tryDeserialize() {
		val dataLength = dataLength
		val stream = CodedInputStream.newInstance(buffer.drop(4).take(dataLength).toArray)
		val result = Message.parseFrom(stream)
		log.info(s"Received message: $result")

		buffer = buffer.drop(4 + dataLength)
		val length = buffer.length
		log.info(s"Remaining $length bytes")
	}
}
