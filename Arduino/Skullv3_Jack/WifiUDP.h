void wifi_setup();
void  wifi_routeMessages();
void wifi_sendPing();
void wifi_broadcast_ip();
void wifi_handshake(OSCMessage &msg, int addrOffset);
void wifi_send_config(OSCMessage &msg, int addrOffset);
void wifi_sendOSCMessage(OSCMessage &msg);
OSCMessage handshake_message();
