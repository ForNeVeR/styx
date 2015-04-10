package ru.org.codingteam.styx.server

import akka.actor.{ActorRef, Actor, ActorLogging}
import akka.pattern.ask
import ru.org.codingteam.styx.LoginDef.Login
import ru.org.codingteam.styx.LoginResultDef.LoginResult
import ru.org.codingteam.styx.ChunkHashDef.ChunkHash
import org.joda.time.DateTime
import scala.collection.mutable.ArrayBuffer
import ru.org.codingteam.styx.data.MessageInfo
import scala.concurrent.Await
import scala.concurrent.duration._
import scala.language.postfixOps
import ru.org.codingteam.styx.ChunkHashResultDef.ChunkHashResult
import ru.org.codingteam.styx.MessageDef.Message
import ru.org.codingteam.styx.MessageResultDef.MessageResult

class ClientActor(val storage: ActorRef) extends Actor with ActorLogging {
	private var state: SynchronizerState = NotConnected

	override def receive = {
		case loginRequest: Login =>
			sender ! processLoginRequest(loginRequest)
		case chunkHash: ChunkHash =>
			sender ! processChunkHash(chunkHash)
		case message: Message =>
			sender ! processMessage(message)
		case UnknownMessage =>
			log.info("Got unknown message")
	}

	private def processLoginRequest(request: Login) = {
		state match {
			case NotConnected =>
				// TODO: Check user credientials
				val status = request.getUsername == "user" && request.getPassword == "password"
				val message = LoginResult.newBuilder().setLogged(status).build()
				if (status) {
					state = Hashing
				}
				message
			case _ =>
				createError("Already logged in")
		}
	}

	private def processChunkHash(request: ChunkHash): com.google.protobuf.Message = {
		state match {
			case Hashing =>
				// TODO: Use proper user name here.
				val futureMessages = ask(
					storage,
					GetMessages(
						"user",
						request.getProtocol,
						request.getUserId,
						new DateTime(request.getTimestamp),
						request.getCount))(1 minute).mapTo[ArrayBuffer[MessageInfo]]
				val messages = Await.result(futureMessages, 1 minute)
				val count = messages.size
				if (count == request.getCount) {
					val hash = calculateHash(messages)
					if (hash == request.getHash) {
						return ChunkHashResult.newBuilder().setPositive(true).build()
					}
				}

				ChunkHashResult.newBuilder().setPositive(false).build()
			case _ =>
				createError("Cannot process hash when not in Hashing state")
		}
	}

	private def processMessage(message: Message): com.google.protobuf.Message = {
		state match {
			case Hashing =>
				// TODO: Use proper user name here.
				val futureStatus = ask(
					storage,
					StoreMessage(
						"user",
						message.getProtocol,
						message.getUserId,
						message.getDirection,
						new DateTime(message.getTimestamp),
						message.getText))(1 minute)
				val status = Await.result(futureStatus, 1 minute)
				status match {
					case Ok => MessageResult.newBuilder().setSuccess(true).build()
					case _ => createError("Cannot store message into a database. Check server log for details.")
				}
			case _ =>
				createError("Cannot process message when not in Hashing state")
		}
	}

	private def createError(reason: String) =
		ru.org.codingteam.styx.ErrorDef.Error.newBuilder().setReason(reason).build()

	private def calculateHash(messages: Iterable[MessageInfo]): Long = {
		// TODO: Use some more complex hash composition technique.
		var hash = 0L
		for (message <- messages) {
			message match {
				case MessageInfo(protocol, contactName, direction, time, text) =>
					hash += protocol.getBytes.size
					hash += text.getBytes.size
					hash += direction.getNumber
			}
		}

		hash
	}
}
