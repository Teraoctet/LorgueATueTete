void wifi_setup()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)delay(5);
  Udp.begin(listenPort);
}

void  wifi_routeMessages()
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

void wifi_sendPing()
{
  if ((unsigned long)(millis() - lastPingTime) > pingTimeInMs)
  {
    OSCMessage pingmsg("/ping");
    pingmsg.add(SKULL_ID);
    pingmsg.add(SKULL_NAME);
    /*
  // get and send local IP
  String ipString = WiFi.localIP().toString();
  char ipChar[ipString.length()+1]; // why +1 ?
  ipString.toCharArray(ipChar, ipString.length()+1);
    pingmsg.add(ipChar);
    */
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
    msg.add(SKULL_NAME);
  // get and send local IP
  String ipString = WiFi.localIP().toString();
  char ipChar[ipString.length()+1]; // why +1 ?
  ipString.toCharArray(ipChar, ipString.length()+1);
    msg.add(ipChar);

  // get and send target IP
  String ipString2 = outIp.toString();
  char ipChar2[ipString2.length()+1]; // why +1 ?
  ipString2.toCharArray(ipChar2, ipString2.length()+1);
    msg.add(ipChar2);
  return msg;
}

void wifi_sendOSCMessage(OSCMessage &msg)
{
  Udp.beginPacket(outIp, outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();
}

