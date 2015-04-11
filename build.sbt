name := "styx-server"

version := "0.1-SNAPSHOT"

mainClass in (Compile, run) := Some("me.fornever.styx.server.Application")

scalaVersion := "2.10.3"

scalacOptions ++= Seq("-unchecked", "-deprecation", "-feature")

libraryDependencies += "com.typesafe.akka" % "akka-actor_2.10" % "2.1.0"

libraryDependencies += "com.google.protobuf" % "protobuf-java" % "2.5.0"

libraryDependencies += "com.h2database" % "h2" % "1.3.170"

libraryDependencies += "joda-time" % "joda-time" % "2.2"

libraryDependencies += "org.joda" % "joda-convert" % "1.2"
