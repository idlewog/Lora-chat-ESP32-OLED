// Includes
#include <Wire.h> 
#include <SSD1306.h> // you need to install the ESP8266 oled driver for SSD1306 

#include <SPI.h>
#include <LoRa.h>    // this is the one by Sandeep Mistry, 
                     
// display descriptor
// Bus I2C
SSD1306 display(0x3c, 4, 15);

// definitions
//SPI defs for screen ?? lora plutot

#define SS  18
#define RST 14
#define DI0 26

// LoRa Settings 
#define BAND                  866E6
#define spreadingFactor       9
#define SignalBandwidth       31.25E3
#define codingRateDenominator 8
#define preambleLength        8

// we also need the following config data:
// GPIO5 —  SX1278 SCK
// GPIO19 — SX1278 MISO
// GPIO27 — SX1278 MOSI
// GPIO18 — SX1278 CS
// GPIO14 — SX1278 RESET
// GPIO26 — SX1278 IRQ(Interrupt Request)


// init des boutons
const int left = 38;
const int right  = 36;
const int valide = 37;
const int go = 13;

// etat des boutons
int sl = 0;
int sr = 0;
int sv = 0;
int sg = 0;

// return to sentmessage
int bg = 0;
int pbg = 0;

// etat précédent des boutons
int psl = 0;
int psr = 0;
int psv = 0;
int psg = 0;

// offset 
int offset = 0;

// misc vars
String  msg;
String  pseudo;

// messages table
String  message[100];
int     msgid=0;


// pseudo flag
int     pset = 0;

// message flag
int     mset = 0;

String  sendMsg;
char    chr;

// key code
int     Ckey = 0;

int     i = 0;

// mode : diplay message (2) vs set message (1)
int dmode = 1;

// jeu de caractère
String carac = "ABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789 @#! -_ <>?";

// rotate string
void rotateString(int offset) {
        int len = carac.length();
       
        offset %= len;
        reverse(carac, 0, len - offset - 1);
        reverse(carac, len - offset, len - 1);
        reverse(carac, 0, len - 1);
        
        //debug
        Serial.print("Len carac :"); Serial.println(len);
        Serial.print("Rotate carac :"); Serial.println(carac);
}

void reverse(String &str, int start, int end) {
        while (start < end) {
            char temp = str[start];
            str[start] = str[end];
            str[end] = temp;
            start++;
            end--;
        }
}

int CodeKeyboard() {
  // return code
  int rcode = 0;
  
  // lecture des boutons
  sl = digitalRead(left);
  sr = digitalRead(right);
  sv = digitalRead(valide);
  sg = digitalRead(go);

  // bouton gauche
  if (sl != psl) {
        if (sl == HIGH) {
          offset = 1;
          rotateString(offset);
        }
        delay(50);
  }
  // save state
  psl = sl;

  // bouton droite
  if (sr != psr) {
          if (sr == HIGH) {
            offset = 47;
            rotateString(offset);
          }
          delay(50);
  }
  // save state
  psr = sr;
  
  if (sv != psv) {
          if (sv == HIGH) {
            rcode = 1;
            Serial.println("Valide");
          } else { 
            Serial.println("Valide up");
          }
          delay(50);
  }  
  // save state
  psv = sv;

  // bouton go
  if (sg != psg) {
        if (sg == HIGH) {
          rcode = 2;
          Serial.print("Go");
        } else {
          Serial.println("Go up");
        }
        delay(50);
  }
  // save state
  psg = sg;

  display.clear();

  // upper string 
  display.drawString(0, 0, carac.substring(0,18));

  // In square choosen letter
  display.setFont(ArialMT_Plain_16);
  display.drawRect(55, 12, 18, 20);
  display.drawString(57, 13, carac.substring(0,1));
  display.setFont(ArialMT_Plain_10);

  // line
  display.drawLine(0, 33, 128, 33);
  return rcode;
}

void setpseudo () {

      while (pset == 0 ) {
          // display keyboard and get char
          Ckey = CodeKeyboard();
    
          // add char to pseudo
          if (Ckey == 1) pseudo += carac.substring(0,1);
                  
          // display pseudo message 
          display.drawString(1, 34, "Pseudo :");
          display.drawString(1, 44, pseudo);
          display.display();
    
          // if valid => pseudo is set 
          if (Ckey == 2) pset = 1;    
      }
        
      // pseudo is set
      display.clear();
      display.setFont(ArialMT_Plain_16);    
      display.drawString(1, 32, "Pseudo set !");
      display.setFont(ArialMT_Plain_10);
      display.display();
  
      // wait 1 sec
      delay(1000);     
}

void setmessage () {

      // add pseudo and :
      message[msgid] += pseudo;
      message[msgid] += " : ";

      while (mset == 0 ) {
          // display keyboard and get char
          Ckey = CodeKeyboard();
    
          // add char to pseudo
          if (Ckey == 1) message[msgid] += carac.substring(0,1);
                  
          // display message 
          display.drawString(1, 34, message[msgid]);
          display.display();
    
          // if valid => message is set 
          if (Ckey == 2) mset = 1;    
      }

      // end line
      message[msgid] += "\n";


      // message is set
      display.clear();
      display.setFont(ArialMT_Plain_16);    
      display.drawString(1, 32, "Message send !");
      display.setFont(ArialMT_Plain_10);
      display.display();
  
      // wait 1 sec
      delay(1000);     
}

void setup() {

  // init des boutons
  pinMode(left, INPUT);
  pinMode(right, INPUT);
  pinMode(valide, INPUT);
  
  // reset the screen
  pinMode(16,OUTPUT);
  digitalWrite(16, LOW); // set GPIO16 low to reset OLED
  delay(50);
  digitalWrite(16, HIGH);
  
  Serial.begin(115200);
  while (!Serial); //If just the the basic function, must connect to a computer

// Initialising the UI will init the display too.
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);

  // init LORA

  // 1 - Com via SPI
  SPI.begin(5,19,27,18);
  LoRa.setPins(SS,RST,DI0);
  
  // 2 - LORA Band
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  // 3 - Spread Factor
  Serial.print("LoRa Spreading Factor: ");
  Serial.println(spreadingFactor);
  LoRa.setSpreadingFactor(spreadingFactor);

  // 4 - Bandwith
  Serial.print("LoRa Signal Bandwidth: ");
  Serial.println(SignalBandwidth);
  LoRa.setSignalBandwidth(SignalBandwidth);

  // 5  - Coding rate
  LoRa.setCodingRate4(codingRateDenominator);

  // 6 - Preamble length 
  LoRa.setPreambleLength(preambleLength);
  
  Serial.println("LoRa Initial OK!");
  
  delay(1000);

  // set pseudo
  setpseudo();
}

void loop() {  

  if (dmode == 1) {
          // get message
          setmessage();
        
          // send message
          LoRa.beginPacket();
          LoRa.print(message[msgid]);
          LoRa.endPacket();
        
          // next message
          msgid++;
                
          // key back to 0 ... not sure if necessary
          Ckey = 0;
          dmode = 2; 
  }

  // Receive a message first...
  int packetSize = LoRa.parsePacket();  
  
  if (packetSize) {
      Serial.print("Packet size : ");Serial.println(packetSize);
      display.clear();
      display.drawString(3, 0, "Received Message!");
      display.display();
    
      while (LoRa.available()) {
          String data = LoRa.readString();
          message[msgid] = data;
          msgid ++;
      }
  }

  // next message (limit of 100)
  if (msgid > 99) msgid = 0;
  
  // display all
  display.clear();

  if (msgid > 3) {
      display.drawString(0, 0, message[msgid-4]);
      display.drawString(2, 10, message[msgid-3]);
      display.drawString(0, 20, message[msgid-2]);
      display.drawString(2, 30, message[msgid-1]);
  } else {
      display.drawString(0, 0, message[0]);
      display.drawString(2, 10, message[1]);
      display.drawString(0, 20, message[2]);
      display.drawString(2, 30, message[3]);
  }
      
  display.display();

  // if valid, back to setmessage
  bg = digitalRead(left);
  // bouton go
  if (bg != pbg) {
        if (bg == HIGH) {
          dmode = 1;
          mset = 0;
        }
         delay(50);
  }
  // save state
  pbg = bg;
    
}


