package ru.org.codingteam.styx.server

import akka.actor.{ActorLogging, Actor}
import org.h2.jdbcx.JdbcConnectionPool
import java.sql.{PreparedStatement, Statement, Connection}
import org.joda.time.DateTime

case class GetMessages(clientName: String, protocol: String, contactUId: String, time: DateTime, count: Int)

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
	}

	private def ensureDatabaseInitialized() {
		if (!checkDatabaseCreated()) {
			createDatabase()
		}
		updateDatabase()
	}

	private def getMessages(clientName: String, protocol: String, contactUId: String, time: DateTime, count: Int) = {

	}

	private def checkDatabaseCreated() = {
        val statement = connection.prepareStatement(
			"select count(*) from information_schema.tables where table_name = ?")
		try {
			statement.setString(1, VersionTableName)
			val count = executeScalar(statement).getOrElse(0)
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
		val statement = connection.prepareStatement(s"select top 1 from $VersionTableName")
		try {
			executeScalar[Int](statement).getOrElse(0)
		} finally {
			statement.close()
		}
	}

	private def setVersion(version: Int) = {
		def insertVersion() = connection.prepareStatement(s"insert into $VersionTableName values(?)")
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
			  | contact varchar,
			  | direction integer,
			  | datetime datetime
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
			executeScalar(statement).getOrElse(0)
		} finally {
			statement.close()
		}
	}
}
