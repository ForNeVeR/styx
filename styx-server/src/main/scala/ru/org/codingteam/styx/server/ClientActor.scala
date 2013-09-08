package ru.org.codingteam.styx.server

import akka.actor.{Actor, ActorLogging}
import ru.org.codingteam.styx.LoginDef.Login
import ru.org.codingteam.styx.LoginResultDef.LoginResult
import com.google.protobuf.Message
import ru.org.codingteam.styx.ChunkHashDef.ChunkHash

class ClientActor extends Actor with ActorLogging {
	private var state: SynchronizerState = NotConnected

	override def receive = {
		case loginRequest: Login =>
			sender ! processLoginRequest(loginRequest)
		case chunkHash: ChunkHash =>
			sender ! processChunkHash(chunkHash)
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

	private def processChunkHash(request: ChunkHash) = {
		state match {
			case Hashing =>
				// TODO: Check hash with the StorageActor.
			case _ =>
				createError("Cannot process hash when not in Hashing state")
		}
	}

	private def createError(reason: String) =
		ru.org.codingteam.styx.ErrorDef.Error.newBuilder().setReason(reason).build()
}
