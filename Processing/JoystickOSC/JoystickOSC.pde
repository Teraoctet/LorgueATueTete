/**
 Basic demonstration of using a joystick.
 
 When this sketch runs it will try and find
 a game device that matches the configuration
 file 'joystick' if it can't match this device
 then it will present you with a list of devices
 you might try and use.
 
 The chosen device requires 2 sliders and 2 buttons.
 */

import org.gamecontrolplus.gui.*;
import org.gamecontrolplus.*;
import net.java.games.input.*;
 
import oscP5.*;
import netP5.*;

ControlIO control;
ControlDevice device;

//ArrayList<ControlButton> buttons = new ArrayList<ControlButton>();
ControlHat hat;
ControlSlider sliderX;
ControlSlider sliderY;


NetAddress myRemoteLocation = new NetAddress("127.0.0.1",12345);
OscMessage msg;

public void setup() {
  size(400, 400);
  // Initialise the ControlIO
  control = ControlIO.getInstance(this);
    device = control.getDevice("Logitech Extreme 3D");
  
  device.getButton("SHADOW").plug(this, "dropShadow", ControlIO.ON_RELEASE);
  for (int i = 0; i < 3 ; i++)
    device.getButton(str(i)).plug(this, "button"+str(i), ControlIO.ON_RELEASE);
    
  hat = device.getHat("pov");
  hat.plug(this, "left", ControlIO.ON_RELEASE);
  
  sliderX = device.getSlider("x");
  sliderY = device.getSlider("x");
}

public void draw() {  
  msg = new OscMessage("/x");
  msg.add(sliderX.getValue());
  OscP5.flush(msg,myRemoteLocation);
  
  msg = new OscMessage("/y");
  msg.add(sliderY.getValue());
  OscP5.flush(msg,myRemoteLocation);
  
  msg = new OscMessage("/hat/left");
  msg.add(hat.left());
  OscP5.flush(msg,myRemoteLocation);
  
  msg = new OscMessage("/hat/right");
  msg.add(hat.right());
  OscP5.flush(msg,myRemoteLocation);
  
  msg = new OscMessage("/hat/up");
  msg.add(hat.up());
  OscP5.flush(msg,myRemoteLocation);
  
  msg = new OscMessage("/hat/down");
  msg.add(hat.down());
  OscP5.flush(msg,myRemoteLocation);
}

public void dropShadow() {
  println("SHADOW");
}

public void left() {
  println("LEFT");
}
public void button0() {
  msg = new OscMessage("/button");
  msg.add(0);
  OscP5.flush(msg,myRemoteLocation);
}

public void button1() {
  msg = new OscMessage("/button");
  msg.add(1);
  OscP5.flush(msg,myRemoteLocation);
}

public void button2() {
  msg = new OscMessage("/button");
  msg.add(1);
  OscP5.flush(msg,myRemoteLocation);
}