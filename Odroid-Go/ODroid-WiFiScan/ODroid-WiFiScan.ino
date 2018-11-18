/*
 *  Wi-Fi Scan
 *  Porting della versione classica di maurizio.conti@fablabromagna.org
 *  Ottobre 2018
 * 
 *  This sketch demonstrates how to scan WiFi networks.
 *  The API is almost the same as with the WiFi Shield library,
 *  the most obvious difference being the different file you need to include:
 */
#include "WiFi.h"
#include <odroid_go.h>

void setup()
{
    // Elenco dei font disponibili
    // https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts
    
    GO.begin();
    GO.lcd.clearDisplay();
    GO.lcd.setFont(&FreeSerif9pt7b);    
    GO.lcd.setTextSize(1);
    GO.lcd.println("");
    GO.lcd.setTextColor(RED);
    GO.lcd.print("FabLab");
    GO.lcd.setTextColor(WHITE);
    GO.lcd.println(" WiFi scan start...");
    GO.update();

    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    GO.lcd.println("ready...");
    GO.update();
    delay(1000);
}

void loop()
{
    GO.begin();
    GO.lcd.clearDisplay();
    GO.lcd.setFont(&FreeSerif9pt7b);    
    GO.lcd.setTextSize(0);
    GO.update();

    GO.lcd.println("");
    GO.lcd.setTextColor(RED);
    GO.lcd.print("FabLab");
    GO.lcd.setTextColor(WHITE);
    GO.lcd.println(" WiFi scan start...");
    GO.update();

    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();

    if (n == 0) {
        GO.lcd.setTextColor(RED);
        GO.lcd.println("no networks found");
    } else {
        GO.lcd.setTextColor(GREEN);
        GO.lcd.print(n);
        GO.lcd.println(" networks found");
        GO.lcd.setTextColor(WHITE);
        
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            GO.lcd.print(i + 1);
            GO.lcd.print(": ");
            GO.lcd.print(WiFi.SSID(i));
            GO.lcd.print(" (");
            GO.lcd.print(WiFi.RSSI(i));
            GO.lcd.print(")");
            GO.lcd.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            delay(10);
        }
    }
    GO.lcd.println("");
    GO.update();

    // Wait a bit before scanning again
    delay(8000);
}
