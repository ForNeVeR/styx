package me.fornever.styx.server

import akka.actor.{ActorLogging, Actor}
import me.fornever.styx.data.MessageInfo
import org.h2.jdbcx.JdbcConnectionPool
import java.sql.{Timestamp, PreparedStatement, Connection}
import org.joda.time.DateTime
import scala.collection.mutable.ArrayBuffer
import ru.org.codingteam.styx.data.MessageInfo
import ru.org.codingteam.styx.MessageDef.Message.Direction

case object Ok
case class GetMessages(clientName: String, protocol: String, contactUId: String, time: DateTime, count: Int)
case class StoreMessage(
	clientName: String,
	protocol: String,
	contactUID: String,
	direction: Direction,
	time: DateTime,
	text: String)

class StorageActor extends Actor with ActorLogging {
	val VersionTableName = "VERSION"

	var connection: Connection = null

	override def preStart() {
		val pool = JdbcConnectionPool.create("jdbc:h2:styx;DB_CLOSE_DELAY=-1", "sa", "")
		connection = pool.getConnection()
		ensureDatabaseInitialized()
	}

	override def receive = {
		case GetMessages(clientName, protocol, contact, time, count) =>
			sender ! getMessages(clientName, protocol, contact, time, count)
		case StoreMessage(clientName, protocol,contactUID, direction, time, text) =>
			sender ! storeMessage(clientName, protocol,contactUID, direction, time, text)
	}

	private def ensureDatabaseInitialized() {
		if (!checkDatabaseCreated()) {
			createDatabase()
		}
		updateDatabase()
	}

	private def getMessages(clientName: String, protocol: String, contactUId: String, time: DateTime, count: Int) = {
		val statement = connection.prepareStatement(
			s"""select top $count direction, datetime, text
			   |from message
			   |where client = ? and protocol = ? and contact = ? and datetime >= ?
			""".stripMargin)
		try {
			statement.setString(1, clientName)
			statement.setString(2, protocol)
			statement.setString(3, contactUId)
			statement.setTimestamp(4, new Timestamp(time.getMillis))
			val resultSet = statement.executeQuery()
			try {
				val messages = new ArrayBuffer[MessageInfo]()
				while (resultSet.next()) {
					val direction = Direction.valueOf(resultSet.getInt("direction"))
					val time = new DateTime(resultSet.getDate("datetime").getTime)
					val text = resultSet.getString("text")
					val message = new MessageInfo(protocol, contactUId, direction, time, text)
					messages += message
				}

				messages
			} finally {
				resultSet.close()
			}
		} finally {
			statement.close()
		}
	}

	private def storeMessage(
		clientName: String,
		protocol: String,
	    contactUID: String,
	    direction: Direction,
	    time: DateTime,
	    text: String) = {
		val statement = connection.prepareStatement(
			"""insert into message(client, protocol, contact, direction, datetime, text)
			  |values (?, ?, ?, ?, ?, ?)
			""".stripMargin)
		try {
			statement.setString(1, clientName)
			statement.setString(2, protocol)
			statement.setString(3, contactUID)
			statement.setInt(4, direction.getNumber)
			statement.setTimestamp(5, new Timestamp(time.getMillis))
			statement.setString(6, text)

			Ok
			// TODO: Catch errors.
		} finally {
			statement.close()
		}
	}

	private def checkDatabaseCreated() = {
        val statement = connection.prepareStatement(
			"select count(*) from information_schema.tables where table_name = ?")
		try {
			statement.setString(1, VersionTableName)
			val count = executeScalar(statement).getOrElse(0L)
			count > 0
		} finally {
			statement.close()
		}
	}

	private def executeScalar[T](statement: PreparedStatement) = {
		try {
			val resultSet = statement.executeQuery()
			try {
				if (!resultSet.next()) {
					None
				} else {
					Some(resultSet.getObject(1).asInstanceOf[T])
				}
			} finally {
				resultSet.close()
			}
		}
	}

	private def createDatabase() {
		val statement = connection.prepareStatement(s"create table $VersionTableName (version integer)")
		try {
			statement.execute()
		} finally {
			statement.close()
		}
	}

	private def updateDatabase() {
		if (getVersion == 0) {
			createMessageTable()
			setVersion(1)
		}
	}

	private def getVersion() = {
		val statement = connection.prepareStatement(s"select top 1 version from $VersionTableName")
		try {
			executeScalar[Int](statement).getOrElse(0)
		} finally {
			statement.close()
		}
	}

	private def setVersion(version: Int) = {
		def insertVersion() = connection.prepareStatement(s"insert into $VersionTableName (version) values(?)")
		def updateVersion() = connection.prepareStatement(s"update $VersionTableName set version = ?")

		val recordCount = getRecordCount(VersionTableName)
		val statement = if (recordCount == 0) insertVersion() else updateVersion()
		try {
			statement.setInt(1, version)
			statement.execute()
		} finally {
			statement.close()
		}
	}

	private def createMessageTable() {
		val statement = connection.prepareStatement(
			"""create table message (
			  | client varchar,
			  | protocol varchar,
			  | contact varchar,
			  | direction integer,
			  | datetime datetime,
			  | text varchar
			  |)""".stripMargin)
		try {
			statement.execute()
		} finally {
			statement.close()
		}
	}

	private def getRecordCount(tableName: String) = {
		val statement = connection.prepareStatement(s"select count(*) from $tableName")
		try {
			executeScalar(statement).getOrElse(0L)
		} finally {
			statement.close()
		}
	}
}
