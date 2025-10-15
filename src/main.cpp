#include "global.hpp"
#include "menu.hpp"

#include "infrared/tvbgone.hpp"
#include "infrared/spammer.hpp"
#include "infrared/universal_remote.hpp"

#include "music/music.hpp"
#include "music/piano.hpp"
#include "music/beep.hpp"

#include "wifi/utils.hpp"
#include "wifi/scan.hpp"
#include "wifi/spam.hpp"
#include "wifi/deauth.hpp"

#include "bluetooth/spam.hpp"
#include "bluetooth/scan.hpp"

#include "rf/tesla.hpp"
#include "rf/jammer.hpp"
#include "rf/identifier.hpp"
#include "rf/scan.hpp"
#include "rf/send.hpp"

#include "headless.h"
#include "settings.h"

#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#include <LittleFS.h>

Menu menu;

Menu menu_wifi;
Menu menu_bluetooth;

Menu menu_infrared;
Menu menu_tvbgone;
Menu menu_universal_remote;

Menu menu_rf;
Menu menu_rf_send;

Menu menu_sound;

Menu menu_settings;

Menu* active_menu = nullptr;

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  Serial.begin(115200);

  bool display_found = true;
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0X3C)) {
    Serial.println("SSD1306 allocation failed, force starting headless...");
    display_found = false;
  }

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(IR_TX, OUTPUT);

  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_CENTER, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);

  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(IR_TX, LOW);

  gpio_set_drive_capability(gpio_num_t(IR_TX), GPIO_DRIVE_CAP_3);
  gpio_set_drive_capability(gpio_num_t(BUZZER_PIN), GPIO_DRIVE_CAP_3);

  if (!LittleFS.begin()) {
    display.clearDisplay();
    Display_PrintCentered("Filesystem\nInitialization\nfailed!");
    display.display();

    delay(1000);
    
    bool last_state = false;
    unsigned long hold_start = millis();

    while (true)
    {
      display.clearDisplay();
      Display_PrintCentered("Hold center\n to format");  
      display.display();

      if (ReadButton(BUTTON_CENTER)) {
        if (!last_state) {
          last_state = true;
          hold_start = millis();
        }

        if (millis() - hold_start > 1000) {
          LittleFS.begin(true);
          ESP.restart();
          return;
        }
      }
      else
      {
        last_state = false;
      }

      delay(100);
    }
  }

  if (!display_found)
  {
    Headless_Setup();
  }
  else
  {
    auto config = ReadConfig("/config.cfg");

    if (config.find("headless") == config.end())
        config["headless"] = "0";

    bool headless = config["headless"] == "1";

    if (headless)
    {
      Headless_Setup();
    }
  }

  menu.AddItem(MenuItem("WiFi", &menu_wifi));
  menu.AddItem(MenuItem("Bluetooth", &menu_bluetooth));
  menu.AddItem(MenuItem("Infrared", &menu_infrared));
  menu.AddItem(MenuItem("Radio", &menu_rf));
  menu.AddItem(MenuItem("Sound", &menu_sound));
  menu.AddItem(MenuItem("Settings", &menu_settings));

  menu_wifi.AddItem(MenuItem("Scan", WiFi_Scan));
  menu_wifi.AddItem(MenuItem("Beacon Spam", WiFi_BeaconSpam));

  menu_bluetooth.AddItem(MenuItem("Scan", BLE_Scan));
  menu_bluetooth.AddItem(MenuItem("Pairing Spam", BLE_Spam));
  
  menu_infrared.AddItem(MenuItem("TV-B-Gone", &menu_tvbgone));
  menu_infrared.AddItem(MenuItem("Spammer", Ir_Spammer));
  menu_infrared.AddItem(MenuItem("Universal Remote", &menu_universal_remote));

  menu_tvbgone.AddItem(MenuItem("EU", TvbGone_Callback_EU));
  menu_tvbgone.AddItem(MenuItem("NA", TvbGone_Callback_NA));

  menu_universal_remote.AddItem(MenuItem("TVs", [&]() { IR_UniversalRemote(REMOTE_TYPE_TV); }));
  menu_universal_remote.AddItem(MenuItem("Projectors", [&]() { IR_UniversalRemote(REMOTE_TYPE_PROJECTOR); }));

  menu_rf.AddItem(MenuItem("Scan", RF_Scan));
  menu_rf.AddItem(MenuItem("Send", RF_Send));
  menu_rf.AddItem(MenuItem("Jammer", RF_Jammer));
  menu_rf.AddItem(MenuItem("Identifier", RF_Identifier));
  menu_rf.AddItem(MenuItem("Tesla Charge Port", RF_TeslaChargePort));

  menu_sound.AddItem(MenuItem("Beep", Beep_Callback));
  menu_sound.AddItem(MenuItem("Music", Music_Callback));
  menu_sound.AddItem(MenuItem("Piano", Piano_Callback));

  menu_settings.AddItem(MenuItem("Headless", [](){
      auto config = ReadConfig("/config.cfg");
      bool current = config["headless"] == "1";
      bool next = !current;
      config["headless"] = next ? "1" : "0";
      WriteConfig("/config.cfg", config);

      display.clearDisplay();
      Display_PrintCentered("Headless:\n%s", next ? "ON" : "OFF");
      display.display();

      delay(500);
      ESP.restart();
  }));

  menu.Revive(nullptr);
  active_menu = &menu;
}

void loop() {
  noTone(BUZZER_PIN);
  digitalWrite(IR_TX, LOW);

  if (!active_menu) return;
  Menu* next = active_menu->HandleButtons();
  if (next) active_menu = next;

  active_menu->Render();
}
