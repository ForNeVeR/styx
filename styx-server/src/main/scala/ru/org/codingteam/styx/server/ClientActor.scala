package ru.org.codingteam.styx.server

import akka.actor.{Actor, ActorLogging}
import ru.org.codingteam.styx.LoginDef.Login
import ru.org.codingteam.styx.LoginResultDef.LoginResult
import com.google.protobuf.Message

class ClientActor extends Actor with ActorLogging {
	private var state: SynchronizerState = NotConnected

	override def receive = {
		case loginRequest: Login =>
			processLoginRequest(loginRequest)
		case UnknownMessage =>
			log.info("Got unknown message")
	}

	private def processLoginRequest(request: Login) {
		val result: Message = state match {
			case NotConnected =>
				// TODO: Check user credientials
				val status = request.getUsername == "user" && request.getPassword == "password"
				val message = LoginResult.newBuilder().setLogged(status).build()
				if (status) {
					state = Hashing
				}
				message
			case _ =>
				val message = ru.org.codingteam.styx.ErrorDef.Error.newBuilder().setReason("Already logged in").build()
				message
		}

		sender ! result
	}
}
