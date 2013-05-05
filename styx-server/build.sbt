name := "styx-server"

version := "0.1-SNAPSHOT"

mainClass in (Compile, run) := Some("ru.org.codingteam.styx.server.Application")

scalaVersion := "2.10.0"

scalacOptions ++= Seq("-unchecked", "-deprecation", "-feature")

libraryDependencies += "com.typesafe.akka" % "akka-actor_2.10" % "2.1.0"

libraryDependencies += "com.google.protobuf" % "protobuf-java" % "2.5.0"
