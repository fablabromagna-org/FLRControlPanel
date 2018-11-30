
/*
 * Client HTTP per Odroid-Go
 * https://wiki.odroid.com/odroid_go/odroid_go
 * 
 * 23 Ottobre 2018
 * maurizio.conti@fablabromagna.org
 * 
 * Warning: la lib ArduinoJson utilizzata è la stable 5.13.3
 * Non usare la 6 in beta ...
 */

#include <Arduino.h>
#include <odroid_go.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <SPI.h>

void ClearScreen();
void Print( String );
void PrintLn( String );
void Menu();

bool PulsanteAPremuto();
void AttendiPulsanteARilasciato();
bool PulsanteBPremuto();
void AttendiPulsanteBRilasciato();
bool PulsanteDownPremuto();
void AttendiPulsanteDownRilasciato();
bool PulsanteUpPremuto();
void AttendiPulsanteUpRilasciato();
void RefreshScreen();
bool NetworkIsBusy();
 
#define USE_SERIAL Serial
#define SCREEN_REFRESH_TIMEOUT 1000
#define NETWORK_TIMEOUT 5000

int valoreManuale = 0;

WiFiMulti wifiMulti;
unsigned long screenTimeOut = 0; 
unsigned long networkTimeOut = 0; 
bool MenuIsVisible = false;
bool MessaggiRossiVisibili = false;

void setup() {

  GO.begin();

  ClearScreen();
  PrintLn( "FabLab 2018!" );

  GO.Speaker.setVolume( 2 );
  GO.Speaker.playMusic( m5stack_startup_music, 25000 );
  GO.Speaker.setVolume( 0 );

  wifiMulti.addAP( "iPhone di Maurizio", "fablabasl2018" );
  wifiMulti.addAP( "ChromeCast", "usr-rimini" );

  PrintLn( "mi connetto... (premi A per reboot)" );

  while( (wifiMulti.run() != WL_CONNECTED) ) {
    delay( 500 );
    Print( "." );
    
    if( GO.BtnA.wasPressed() ) 
      return;    
  }
  
  ClearScreen();
  PrintLn( "WiFi connected" );
  GO.lcd.println( WiFi.localIP() );
}

void loop() {

  RefreshScreen();

  if( (wifiMulti.run() == WL_CONNECTED) ) {
    if( !NetworkIsBusy() ) {
      if( PulsanteAPremuto() ) {
      
        ClearScreen();
        PrintLn( "try reading..." );
  
        HTTPClient http;
        http.begin( "http://dweet.io/get/latest/dweet/for/lab226" );
  
        int httpCode = http.GET();
        
        if( httpCode > 0 ) {
  
          // file found at server
          if(httpCode == HTTP_CODE_OK) {
              
            String payload = http.getString();
            const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60;
            DynamicJsonBuffer jsonBuffer(capacity);
            JsonObject& root = jsonBuffer.parseObject(payload);
  
            if ( !root.success() )
              PrintLn( "Errore di connessione..." );
            
              // Estrai i valori di dweet
              String strTemperatura = root["with"][0]["content"]["temperatura"].as<char*>();
              Serial.println(F( "Response:" ));
              Serial.println( strTemperatura.c_str() );

              
              if( !strTemperatura )
                strTemperatura = "0.0";
      
              float v = strTemperatura.toFloat();
              valoreManuale = (int) v;
              
              ClearScreen();
              PrintLn( "Temperatura letta dal Web: " + strTemperatura );
            } 
            else
              PrintLn( http.errorToString(httpCode).c_str() );
          }
          else
            PrintLn( "Non trovo la rete..." );
          
          http.end();
          networkTimeOut = millis() + NETWORK_TIMEOUT;
       }
           
      if( PulsanteBPremuto() ) {
        
        ClearScreen();
        PrintLn( "try sending..." );
            
        HTTPClient http;
        http.begin( "http://dweet.io/dweet/for/lab226?temperatura=" + String(valoreManuale) );
  
        int httpCode = http.GET();
  
        if( httpCode > 0 ) {
  
          // file found at server
          if( httpCode == HTTP_CODE_OK ) {
                
            String payload = http.getString();
            const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60;
            DynamicJsonBuffer jsonBuffer(capacity);
            JsonObject& root = jsonBuffer.parseObject(payload);
  
            if ( !root.success() )
                PrintLn( "Non spedito!!" );
            else
                PrintLn( String("Spedito valore ") + String(valoreManuale) + String("\n") );
          }
        }
  
        http.end();      
        networkTimeOut = millis() + NETWORK_TIMEOUT;
      }  
    }
  }

//
// FINE main
//

  if( GO.BtnStart.wasPressed() ) {
    ClearScreen();
    //GO.Speaker.playMusic(m5stack_startup_music, 25000);
    GO.update();
  }

  if( PulsanteUpPremuto() ){

    valoreManuale++;
    if( valoreManuale > 255 )
      valoreManuale = 255;
      
    ClearScreen();
    PrintLn( String("Valore: ") + String(valoreManuale) + String("\n") );
    AttendiPulsanteUpRilasciato();
  }
 
  if( PulsanteDownPremuto() ) { 

    valoreManuale--;
    if( valoreManuale < 0 )
      valoreManuale = 0;

    ClearScreen();
    PrintLn( String("Valore: ") + String(valoreManuale) + String("\n") );
    AttendiPulsanteDownRilasciato();
  }
}

void ClearScreen()
{
  GO.lcd.clearDisplay();
  GO.lcd.setFont(&FreeSerif12pt7b);  
  GO.lcd.setTextSize(0);
  GO.lcd.setCursor(0, 18);
  GO.update();
}

void Print( String msg )
{
  GO.lcd.print( msg );
  GO.update();
  screenTimeOut = millis() + 2000;
  MenuIsVisible = false;
}

void PrintLn( String msg )
{
  Print( msg + "\n" );  
}

bool PulsanteDownPremuto(){
  GO.update();
  return (GO.JOY_Y.isAxisPressed() == 1) ? true : false;
}

void AttendiPulsanteDownRilasciato(){
  while( PulsanteDownPremuto() ) {}
}

bool PulsanteUpPremuto(){
  GO.update();
  return (GO.JOY_Y.isAxisPressed() == 2) ? true : false;
}

void AttendiPulsanteUpRilasciato(){
  while( PulsanteUpPremuto() ) {}
}

bool PulsanteAPremuto(){
  GO.update();
  return GO.BtnA.wasPressed();
}

void AttendiPulsanteARilasciato(){
  while( PulsanteAPremuto() ) {}
}

bool PulsanteBPremuto(){
  GO.update();
  return (bool) GO.BtnB.wasPressed();
}

void AttendiPulsanteBRilasciato(){
  while( PulsanteBPremuto() ) {}
}

void RefreshScreen()
{  
  MessaggiRossiVisibili = NetworkIsBusy() ;
  if( (millis() > screenTimeOut) && !MenuIsVisible ) {
    ClearScreen();
    Menu();
  }
}

void Menu() 
{
  GO.lcd.setTextColor(WHITE);    
  PrintLn("Menu...");

  if( MessaggiRossiVisibili ) {
    GO.lcd.setTextColor(RED);
  }    
  else{
    GO.lcd.setTextColor(GREEN);    
  }
  
  PrintLn("A button load from web");
  PrintLn("B button save to web");
  
  GO.lcd.setTextColor(WHITE);    
  PrintLn("UP - move value up");
  PrintLn("DOWN - move value down");

  MenuIsVisible = true;

  if( MessaggiRossiVisibili ) {
    GO.lcd.setTextColor(RED);
    PrintLn("\nnetwork busy...");
  }
}

bool NetworkIsBusy(){
  return (millis() > networkTimeOut) ? false : true;
}

