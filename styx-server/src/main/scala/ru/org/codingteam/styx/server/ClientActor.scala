package ru.org.codingteam.styx.server

import akka.actor.{Actor, ActorLogging}
import akka.util.ByteString
import akka.actor.IO.{SocketHandle, Closed, Read}
import com.google.protobuf.CodedInputStream
import java.nio.ByteBuffer
import ru.org.codingteam.styx.MessageDef.Message
import ru.org.codingteam.styx.MessageTypeDef.MessageType
import ru.org.codingteam.styx.LoginDef.Login

class ClientActor extends Actor with ActorLogging {

	private var buffer = ByteString()
	private var state = NotConnected

	override def preStart() {
		state = NotConnected
		buffer = ByteString()
	}

	def receive = {
		case Read(socket, bytes) =>
			log.info("Received incoming data from socket")
			receiveBytes(bytes)

		case Closed(socket: SocketHandle, cause) =>
			log.info("Socket has closed, cause: " + cause)
			state = NotConnected
			context.stop(self)
	}

	private def dataLength = getInt(buffer)
	private def messageType = MessageType.valueOf(getInt(buffer.drop(4)))

	private def receiveBytes(bytes: ByteString) {
		buffer = buffer ++ bytes
		val length = buffer.length
		log.info(s"Buffer length = $length bytes")
		if (length >= 8) {
			log.info(s"Data length = $dataLength bytes")
			log.info(s"Message type = $messageType")
		    if (length >= dataLength + 8) {
			    log.info("Deserializing message")
			    tryDeserialize()
		    }
		}
	}

	private def tryDeserialize() {
		val dataLength = this.dataLength
		val stream = CodedInputStream.newInstance(buffer.drop(4).take(dataLength).toArray)
		val result = messageType match {
			case MessageType.LoginRequest => Login.parseFrom(stream)
			case _ => "Unknown"
		}

		log.info(s"Received message: $result")

		buffer = buffer.drop(8 + dataLength)
		val length = buffer.length
		log.info(s"Remaining $length bytes")
	}

	private def getInt(packet: ByteString) = {
		val array = packet.take(4).toArray
		ByteBuffer.wrap(array).getInt
	}
}
