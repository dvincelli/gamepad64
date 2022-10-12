#include "hidjoystickrptparser.h"

JoystickReportParser::JoystickReportParser(JoystickEvents *evt) :
joyEvents(evt),
oldHat(0xDE),
oldButtons(0) {
        for (uint8_t i = 0; i < RPT_GEMEPAD_LEN; i++)
                oldPad[i] = 0xD;
}


void JoystickReportParser::Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf) {
        bool match = true;

        // Checking if there are changes in report since the method was last called
        for (uint8_t i = 0; i < RPT_GEMEPAD_LEN; i++)
                if (buf[i] != oldPad[i]) {
                        match = false;
                        break;
                }

        // Calling Game Pad event handler
        if (!match && joyEvents) {
                joyEvents->OnGamePadChanged((const GamePadEventData*)buf);

                for (uint8_t i = 0; i < RPT_GEMEPAD_LEN; i++) oldPad[i] = buf[i];
        }

        uint8_t hat = (buf[5] & 0xF);

        // Calling Hat Switch event handler
        if (hat != oldHat && joyEvents) {
                joyEvents->OnHatSwitch(hat);
                oldHat = hat;
        }

        uint16_t buttons = (0x0000 | buf[6]);
        buttons <<= 4;
        buttons |= (buf[5] >> 4);
        uint16_t changes = (buttons ^ oldButtons);

        // Calling Button Event Handler for every button changed
        if (changes) {
                for (uint8_t i = 0; i < 0x0C; i++) {
                        uint16_t mask = (0x0001 << i);

                        if (((mask & changes) > 0) && joyEvents)
                                if ((buttons & mask) > 0)
                                        joyEvents->OnButtonDn(i + 1);
                                else
                                        joyEvents->OnButtonUp(i + 1);
                }
                oldButtons = buttons;
        }
}

void JoystickEvents::OnGamePadChanged(const GamePadEventData *evt) {
   int upPin = 2;
   int downPin = 5;
   int leftPin = 4;
   int rightPin = 3;
   int buttonPin = 6;
   

        if (evt->X == 0x00) {
          // LEFT
          // set RIGHT floating INPUT
          // set LEFT OUTPUT LOW
          pinMode(rightPin, INPUT);
          
          digitalWrite(leftPin, LOW);          
          pinMode(leftPin, OUTPUT);
        } else if (evt->X == 0xFF) {
          // RIGHT
          // Set LEFT floating INPUT
          // set RIGHT OUTPUT LOW
          pinMode(leftPin, INPUT);
          
          digitalWrite(rightPin, LOW);          
          pinMode(rightPin, OUTPUT);

        } else { // 0x80
          pinMode(leftPin, INPUT);
          pinMode(rightPin, INPUT);
          // MIDDLE
          // Set LEFT floating INPUT
          // Set RIGHT floating INPUT
        }
        
        if (evt->Y == 0x00) {
          // UP
          // set DOWN floating INPUT
          // set UP OUTPUT LOW
          pinMode(downPin, INPUT);

          digitalWrite(upPin, LOW);
          pinMode(upPin, OUTPUT);
        } else if (evt->Y == 0xFF) {
          // DOWN
          // set UP floating INPUT
          // set DOWN OUTPUT LOW
          pinMode(upPin, INPUT);

          digitalWrite(downPin, LOW);
          pinMode(downPin, OUTPUT);
        } else { // 0x80
          // Set UP floating INPUT
          // Set DOWN floating INPUT
          pinMode(upPin, INPUT);
          pinMode(downPin, INPUT);
          
        }
        
        switch (evt->Z1) {
          case 0x01:
            // Copy UP
            // set DOWN floating INPUT
            pinMode(downPin, INPUT);
            
            digitalWrite(upPin, LOW);
            pinMode(upPin, OUTPUT);            
     
            break;
          case 0x02:
            // Set BUTTON OUTPUT LOW
            digitalWrite(buttonPin, LOW);
            pinMode(buttonPin, OUTPUT);

            break;
          case 0x04:
            break;
          case 0x08:
            break;
          case 0x10:
            break;
          case 0x20:
            break;
          case 0x40:
            break;
          case 0x80:
            break;
          default: // case 0x00:
            // SET BUTTON floating INPUT
            pinMode(buttonPin, INPUT);
            digitalWrite(buttonPin, LOW);

        }
        /*
        Serial.print("X1: ");
        PrintHex<uint8_t > (evt->X, 0x80);
        Serial.print("\tY1: ");
        PrintHex<uint8_t > (evt->Y, 0x80);
        Serial.print("\tX2: ");
        PrintHex<uint8_t > (evt->Z1, 0x80);
        Serial.print("\tY2: ");
        PrintHex<uint8_t > (evt->Z2, 0x80);
        Serial.print("\tRz: ");
        PrintHex<uint8_t > (evt->Rz, 0x80);
        Serial.println("");
        */
}

void JoystickEvents::OnHatSwitch(uint8_t hat) {
  /*
        Serial.print("Hat Switch: ");
        PrintHex<uint8_t > (hat, 0x80);
        Serial.println("");
  */
}

void JoystickEvents::OnButtonUp(uint8_t but_id) {
  /*
        Serial.print("Up: ");
        Serial.println(but_id, DEC);
  */
}

void JoystickEvents::OnButtonDn(uint8_t but_id) {
  /*
        Serial.print("Dn: ");
        Serial.println(but_id, DEC);
  */
}
