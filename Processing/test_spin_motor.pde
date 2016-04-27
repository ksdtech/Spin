import processing.serial.*;

Serial myPort;
long lastTime;
int sendCount;

Serial openUsbSerial(int baudRate) {
  // On a Mac Serial.list() returns content of /dev/{cu,tty}.*.
  // Before connecting Arduino
  // [0] "/dev/cu.Bluetooth-Incoming-Port"
  // [1] "/dev/cu.Bluetooth-Modem"
  // [2] "/dev/tty.Bluetooth-Incoming-Port"
  // [3] "/dev/tty.Bluetooth-Modem"
  // 
  // After connecting Arduino:
  // [0] "/dev/cu.Bluetooth-Incoming-Port"
  // [1] "/dev/cu.Bluetooth-Modem"
  // [2] "/dev/cu.usbmodemfd121"
  // [3] "/dev/tty.Bluetooth-Incoming-Port"
  // [4] "/dev/tty.Bluetooth-Modem"
  // [5] "/dev/tty.usbmodemfd121"
  //
  // Open whatever port is the one you're using.
  String[] ports = Serial.list();
  for (int i = 0; i < ports.length; i++) {
    String port = ports[i];
    if (port.indexOf("cu.usbmodem") > 0) {
      println("Opening USB Serial port on " + port + " at " + baudRate);
      return new Serial(this, port, baudRate);
    }
  }
  println("No cu.usbmodem port found");
  return null;
}

void setup() {
  myPort = openUsbSerial(9600);
  lastTime = 0;
  sendCount = 0;
}

void draw() {
  if (myPort != null) {
    while (myPort.available() > 0) {
      int inByte = myPort.read();
      println("Received " + inByte);
    }
    if (lastTime == 0 || millis() - lastTime > 20000) {
      String message = "S";
      println("Sending " + message);
      myPort.write(message);
      lastTime = millis();
      sendCount = sendCount + 1;
      if (sendCount >= 2) {
        println("Exiting");
        exit();
      }
    }
  }
}