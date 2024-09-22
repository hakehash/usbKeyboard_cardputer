/**
 * @file usbKeyboard.ino
 * @author SeanKwok (shaoxiang@m5stack.com)
 * @brief M5Cardputer USB Keyboard test
 * @version 0.1
 * @date 2023-10-13
 *
 *
 * @Hardwares: M5Cardputer
 * @Platform Version: Arduino M5Stack Board Manager v2.0.7
 * @Dependent Library:
 * M5GFX: https://github.com/m5stack/M5GFX
 * M5Unified: https://github.com/m5stack/M5Unified
 */

#include "M5Cardputer.h"
#include "USB.h"
#include "USBHIDKeyboard.h"
USBHIDKeyboard Keyboard;

void setup() {
    auto cfg = M5.config();
    M5Cardputer.begin(cfg, true);
    M5Cardputer.Display.setRotation(1);
    M5Cardputer.Display.setTextColor(GREEN);
    M5Cardputer.Display.setTextDatum(middle_center);
    M5Cardputer.Display.setTextFont(&fonts::Orbitron_Light_24);
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.drawString("USB Keyboard",
                                   M5Cardputer.Display.width() / 2,
                                   M5Cardputer.Display.height() / 2);
    Keyboard.begin();
    USB.begin();
}

void loop() {
    M5Cardputer.update();
    // max press 3 button at the same time
    if (M5Cardputer.Keyboard.isChange()) {
        if (M5Cardputer.Keyboard.isPressed()) {
            Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
            // for (auto i : status.word) {
            //     Keyboard.press(i);
            // }
            KeyReport report = {0};
            report.modifiers = status.modifiers;
            uint8_t index    = 0;
            for (auto i : status.hid_keys) {
                auto fixedI = i;
                if(status.fn){
                    switch(i)
                    {
                        //https://wiki.onakasuita.org/pukiwiki/?HID%2F%E3%82%AD%E3%83%BC%E3%82%B3%E3%83%BC%E3%83%89
                        case 0x36:fixedI = 0x50;break; //, LeftArrow
                        case 0x37:fixedI = 0x51;break; //. DownArrow
                        case 0x33:fixedI = 0x52;break; //; UpArrow
                        case 0x38:fixedI = 0x4F;break; /// RightArrow
                        case 0x35:fixedI = 0x29;break; //` Escape
                        case 0x2A:fixedI = 0x4C;break; //  Delete
                    }
                }
                report.keys[index] = fixedI;
                index++;
                if (index > 5) {
                    index = 5;
                }
            }
            Keyboard.sendReport(&report);
            // Keyboard.releaseAll();

            // only text for display
            String keyStr = "";
            for (auto i : status.word) {
                if (keyStr != "") {
                    keyStr = keyStr + "+" + i;
                } else {
                    keyStr += i;
                }
            }

            if (keyStr.length() > 0) {
                M5Cardputer.Display.clear();
                M5Cardputer.Display.drawString(
                    keyStr, M5Cardputer.Display.width() / 2,
                    M5Cardputer.Display.height() / 2);
            }

        } else {
            Keyboard.releaseAll();
            M5Cardputer.Display.clear();
            M5Cardputer.Display.drawString("USB Keyboard",
                                           M5Cardputer.Display.width() / 2,
                                           M5Cardputer.Display.height() / 2);
        }
    }
}
