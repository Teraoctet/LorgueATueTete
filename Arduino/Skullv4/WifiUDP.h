void wifi_setup();
void  wifi_route_messages();
void wifi_send_ping();
void wifi_broadcast_ip();
void wifi_handshake(OSCMessage &msg, int addrOffset);
void wifi_send_config(OSCMessage &msg, int addrOffset);
void wifi_sendOSCMessage(OSCMessage &msg);
OSCMessage handshake_message();
