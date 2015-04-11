package me.fornever.styx

import com.typesafe.config.ConfigFactory

object Configuration {
  
  private val config = ConfigFactory.load()

  def host = config.getString("styx.host")
  def port = config.getInt("styx.port")
}
