package ru.org.codingteam.styx.server

import akka.actor.{Props, ActorRef, Actor, ActorLogging}
import akka.util.ByteString
import akka.actor.IO.{Handle, SocketHandle, Closed, Read}
import com.google.protobuf.{Message, CodedInputStream}
import java.nio.ByteBuffer
import ru.org.codingteam.styx.MessageTypeDef.MessageType
import ru.org.codingteam.styx.LoginDef.Login
import ru.org.codingteam.styx.LoginResultDef.LoginResult

class ClientConnectionActor extends Actor with ActorLogging {
	private var clientSocket: Option[Handle] = None
	private var buffer = ByteString()
	private var clientActor: ActorRef = null

	override def preStart() {
		clientActor = context.actorOf(Props[ClientActor])
	}

	def receive = {
		case Read(socket, bytes) =>
			log.info("Received incoming data from socket")
			clientSocket = Some(socket)
			receiveBytes(bytes)

		case Closed(socket: SocketHandle, cause) =>
			log.info("Socket has closed, cause: " + cause)
			context.stop(self)

		case message: Message =>
			sendMessage(message)
	}

	private def messageType = MessageType.valueOf(getInt(buffer))
	private def dataLength = getInt(buffer.drop(4))

	private def receiveBytes(bytes: ByteString) {
		buffer = buffer ++ bytes
		val length = buffer.length
		log.info(s"Buffer length = $length bytes")
		if (length >= 8) {
			log.info(s"Data length = $dataLength bytes")
			log.info(s"Message type = $messageType")
		    if (length >= dataLength + 8) {
			    log.info("Deserializing message")
			    val message = deserialize()
			    clientActor ! message
		    }
		}
	}

	private def sendMessage(message: Message) = {
		val messageType = getMessageType(message)
		val length = message.getSerializedSize
		val bytes = ByteBuffer.allocate(8 + length).putInt(messageType).putInt(length).array
		val byteString = ByteString.fromArray(bytes, 0, bytes.length) ++ message.toByteString.toByteArray

		log.info(s"Sending message $message")

		val socket = clientSocket.get
		socket.asWritable.write(byteString)
	}

	private def deserialize() = {
		val dataLength = this.dataLength
		val stream = CodedInputStream.newInstance(buffer.drop(8).take(dataLength).toArray)
		val result = messageType match {
			case MessageType.LoginRequest => Login.parseFrom(stream)
			case _ => UnknownMessage
		}

		log.info(s"Received message: $result")

		buffer = buffer.drop(8 + dataLength)
		val length = buffer.length
		log.info(s"Remaining $length bytes")

		result
	}

	private def getInt(packet: ByteString) = {
		val array = packet.take(4).toArray
		ByteBuffer.wrap(array).getInt
	}

	private def getMessageType(message: Message) = {
		message match {
			case m: ru.org.codingteam.styx.ErrorDef.Error =>
				MessageType.ProtocolError_VALUE
			case m: LoginResult =>
				MessageType.LoginRequest_VALUE
		}
	}
}
