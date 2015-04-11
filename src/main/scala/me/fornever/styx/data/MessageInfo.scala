package me.fornever.styx.data

import ru.org.codingteam.styx.MessageDef.Message.Direction
import org.joda.time.DateTime

case class MessageInfo(protocol: String, contactName: String, direction: Direction, time: DateTime, text: String)
