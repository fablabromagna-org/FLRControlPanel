
/*
 * Client HTTP per Odroid-Go
 * 
 * 23 Ottobre 2018
 * maurizio.conti@fablabromagna.org
 * 
 */
#include <Arduino.h>
#include <odroid_go.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <SPI.h>

#define USE_SERIAL Serial

WiFiMulti wifiMulti;

  void setup() {

    GO.begin();

    GO.lcd.clearDisplay();
    //GO.lcd.setFont(&FreeSerifBold24pt7b);
    //GO.lcd.setFont(&FreeSans12pt7b);
    GO.lcd.setFont(&FreeSerif12pt7b);
    
    GO.lcd.setTextSize(0);
    GO.lcd.println("");
    GO.lcd.println("FabLab!!!");
    GO.update();
  
    //GO.Speaker.setVolume(2);
    //GO.Speaker.playMusic(m5stack_startup_music, 25000);
    //GO.Speaker.setVolume(0);

    wifiMulti.addAP("iPhone di Maurizio", "fablabasl2018");
    //wifiMulti.addAP("NuovoBuscone", "Prova1234");

    GO.lcd.print( "mi connetto..." );
    GO.update();

    while( (wifiMulti.run() != WL_CONNECTED) ) {
      delay(1000);
      GO.lcd.print( "." );
      GO.update();
      
      if(GO.BtnA.wasPressed()) 
        return;
    
    }
    
    GO.lcd.clearDisplay();
    GO.lcd.println("");
    GO.lcd.println("WiFi connected");
    GO.lcd.println(WiFi.localIP());

    GO.lcd.println("");
    GO.lcd.println("Premi pulsante A per partire.");
    GO.update();

    while(!GO.BtnA.wasPressed()) {
      GO.update();
    }

    
    while(GO.BtnA.wasPressed()) {
      GO.update();
    }
}

void loop() {

    // wait for WiFi connection
    if( (wifiMulti.run() == WL_CONNECTED) ) {

        HTTPClient http;
        http.begin("http://dweet.io/get/latest/dweet/for/lab226"); //HTTP

        int httpCode = http.GET();

        if( httpCode > 0 ) {

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                
                String payload = http.getString();
                const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60;
                DynamicJsonBuffer jsonBuffer(capacity);
                JsonObject& root = jsonBuffer.parseObject(payload);

                if (!root.success())
                  GO.lcd.println( "Non va!!" );
                
                // Estrai i valori di dweet
                Serial.println(F("Response:"));
                Serial.println(F(root["with"][0]["content"]["temperatura"].as<char*>()));
                String strTemperatura = root["with"][0]["content"]["temperatura"].as<char*>();

                if( strTemperatura )
                  double Temperatura = strTemperatura.toFloat();
                else
                  strTemperatura = "--";

                GO.lcd.clearDisplay();
                GO.lcd.setFont(&FreeSerifBold24pt7b);
                GO.lcd.setTextSize(1);
                GO.lcd.setCursor(100, 130);
                GO.lcd.println( strTemperatura );
                GO.update();
            } 
            else
              //USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
              GO.lcd.println( http.errorToString(httpCode).c_str() );
        }
        else
            GO.lcd.println( "Non trovo la rete..." );

        http.end();
        delay(1500);
    }
    
  
    if(GO.BtnA.wasPressed()) {
      GO.lcd.printf("wasPressed: A \r\n");
      //GO.Speaker.beep();
    }
   
    if(GO.BtnB.wasPressed()) {
      GO.lcd.printf("wasPressed: B \r\n");
      //GO.Speaker.tone(3000, 200);
    }
   
    if(GO.BtnStart.wasPressed()) {
      GO.lcd.printf("wasPressed: Start \r\n");
      //GO.Speaker.playMusic(m5stack_startup_music, 25000);
    }
   
    GO.update();
}
