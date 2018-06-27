void wifi_setup()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)leds_blinkAll(50, 0, 0, 2);
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

      bundleIN.route("/servo", setServo);
      bundleIN.route("/led", setLed);
      bundleIN.route("/ledActive", ledsActive);
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

    // get and send local IP
    IPAddress ip = WiFi.localIP();
    String ipString = String(int(ip[0])) + "." + String(int(ip[1])) + "." + String(int(ip[2])) + "." + String(int(ip[3])) + " ";
    int len = ipString.length();
    char ipChar[len];
    ipString.toCharArray(ipChar, len);

    OSCMessage handshakemsg("/handshake");
    handshakemsg.add(SKULL_ID);
    handshakemsg.add(SKULL_NAME);
    handshakemsg.add(ipChar);
    wifi_sendOSCMessage(handshakemsg);
  }
  leds_flash();
}

void wifi_sendOSCMessage(OSCMessage &msg)
{
  Udp.beginPacket(outIp, outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();
}

