void wifi_setup()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)delay(5);
  Udp.begin(listenPort);
}

void  wifi_route_messages()
{
  OSCMessage bundleIN;
  int size;

  if ( (size = Udp.parsePacket()) > 0)
  {
    while (size--)
      bundleIN.fill(Udp.read());

    if (!bundleIN.hasError())
    {
      bundleIN.route("/handshake", wifi_handshake);
      bundleIN.route("/config", wifi_send_config);
      bundleIN.route("/servo", setServo);
    }
  }
}

void wifi_send_ping()
{
  if ((unsigned long)(millis() - lastPingTime) > pingTimeInMs)
  {
    OSCMessage pingmsg("/ping");
    pingmsg.add(SKULL_ID);
    pingmsg.add(to_char_array(SKULL_NAMES[SKULL_ID]));
    
    // get and send local IP
    pingmsg.add(to_char_array(WiFi.localIP().toString()));
    
    wifi_sendOSCMessage(pingmsg);
    lastPingTime = millis();
  }
}

void wifi_handshake(OSCMessage &msg, int addrOffset)
{
  // exchange IP addresses
  if (msg.isInt(0) && msg.isInt(1) && msg.isInt(2) && msg.isInt(3))
  { 
    // set remote IP
    outIp[0] = msg.getInt(0);
    outIp[1] = msg.getInt(1);
    outIp[2] = msg.getInt(2);
    outIp[3] = msg.getInt(3);

    // store last byte of ip address
    EEPROM.write(0, (byte)outIp[3]);
    EEPROM.commit();
  }
}

void wifi_broadcast_ip()
{
  OSCMessage msg = handshake_message();
    
  Udp.beginPacket("192.168.43.255", outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();
}

void wifi_send_config(OSCMessage &msg, int addrOffset)
{
  OSCMessage m = handshake_message();
  wifi_sendOSCMessage(m);
}

OSCMessage handshake_message()
{
  OSCMessage msg("/handshake");
  msg.add(SKULL_ID);
  msg.add(to_char_array(SKULL_NAMES[SKULL_ID]));
  msg.add(to_char_array(WiFi.localIP().toString()));

  // get and send target IP
  msg.add(to_char_array(outIp.toString()));
  return msg;
}

void wifi_sendOSCMessage(OSCMessage &msg)
{
  Udp.beginPacket(outIp, outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();
}

