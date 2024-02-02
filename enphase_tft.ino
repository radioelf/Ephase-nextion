/*
Captura de datos del Envoy de Enphase (V. <7) por HTTP  y envió a pantalla nextion

*/
#if !defined(ESP32)
#error "Codigo SOLO para ESP32-S2"
#endif

#define DebugSerial 1
// pin led estado (D4)
#define LedPin 2

#include <WiFi.h>
#include <ArduinoOTA.h>
#include <time.h>
#include "envoy.h"
#include "nextion.h"

// Uptime overflow
#define UPTIME_OVERFLOW 4294967295UL
const char *sofVersion = "0.1.0";  //2/2/24

// Nombre del punto de accesos a conectar
const char *ssid = "wifi";
// Contraseña para el punto de accesos a conectar
const char *password = "XXXXXX";
// True si la red WiFi está conectada
bool NetworkFound = false;
// Configuración ip estática
IPAddress local_IP(192, 168, x, xx);
IPAddress gateway(192, 168, x, xx);
IPAddress subnet(255, 255, 255, 0);
IPAddress DNS1(192, 168, x, xx);
bool APmode = false;
// Nombre del punto de acceso si falla la conexión a la red WiFi
#define NAME "TftEnphase"
#define PASS "xxxxxxx"

//************************************************************************************
// Inicializamos OTA
//************************************************************************************
bool InitOTA() {
  // Port defaults
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_SPIFFS
      type = "filesystem";
    }
  });
  ArduinoOTA.onEnd([]() {});
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {});
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.println(error);
    return false;
  });
  ArduinoOTA.begin();
  return true;
}
//************************************************************************************
// Activamos y configuramos el modo AP
//************************************************************************************
void modeAP() {
  // Canal RF 6, ISSD ON, 1 conexión
  // Este ESP32 será un AP
  WiFi.softAP(NAME, PASS, 9, 0, 1);
  NetworkFound = false;
  APmode = true;
#if DebugSerial
  debugPrintln(String(F("MODO AP HABILITADO!!")));
#endif
}
//************************************************************************************
// Conectar a WiFi usando el SSID
// Si la conexión falla, se crea un AP y la función devuelve falso.
//************************************************************************************
bool connectwifi() {
  // si NO estamos en modo AP
  if (!APmode) {
    // Si todavía se conserva la conexión antigua
    WiFi.softAPdisconnect(true);
  }
  APmode = false;
  WiFi.config(local_IP, gateway, subnet, DNS1);
  // Conectar a SSID único
  WiFi.begin(ssid, password);
  uint8_t connetCount = 0;
  // Tratar de conectar
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    delay(1000);
    digitalWrite(LedPin, !digitalRead(LedPin));
    if (++connetCount == 30)
      break;
  }
  // Si no conectamos
  if (connetCount == 30) {
    if (!APmode) {
      modeAP();
      // Error al conectar, configuración AP local
      return false;
    }
  } else {
    delay(30);
    APmode = false;
  }
#if DebugSerial
  debugPrintln(String(F("WIFI: Conexión OK con IP: ")) + WiFi.localIP().toString());
  debugPrintln(String(F("WIFI: mascara de subred: ")) + WiFi.subnetMask().toString());
  debugPrintln(String(F("WIFI: gateway: ")) + WiFi.gatewayIP().toString());
  debugPrintln(String(F("WIFI: DNS: ")) + WiFi.dnsIP().toString());
  debugPrintln(String(F("WIFI: MAC ESP: ")) + WiFi.macAddress().c_str());
  debugPrintln(String(F("WIFI: BSSID: ")) + WiFi.BSSIDstr().c_str());
  debugPrintln(String(F("WIFI: Nombre SSID: ")) + WiFi.SSID().c_str());
  debugPrintln(String(F("WIFI: CH: ")) + WiFi.channel());
  debugPrintln(String(F("WIFI: RSSI: ")) + WiFi.RSSI());
#endif
  return true;
}
//************************************************************************************
// Configuración conexión WIFI
//************************************************************************************
bool confiWifi() {
  // Este ESP32 es una estación
  WiFi.mode(WIFI_STA);
  // No se guardar SSID y contraseña
  WiFi.persistent(false);
  // Después de reiniciar el router todavía podría estar conectado
  WiFi.disconnect();
  delay(1000);
  //conectar a red WiFi
  NetworkFound = connectwifi();
  return NetworkFound;
}
//************************************************************************************
// Configuración conexión WIFI
//************************************************************************************
void setup() {
  // configuramos GPIOs
  pinMode(LedPin, OUTPUT);
  digitalWrite(LedPin, LOW);

  delay(500);
  Serial.begin(115200);  // TXD 43, RXD 44 defecto para ESP32-S2

  // Conectar a wifi y MQTT
  NetworkFound = confiWifi();
  if (!NetworkFound) {
    uint8_t x = 0;
    do {
      digitalWrite(LedPin, !digitalRead(LedPin));
      delay(500);
    } while (++x != 4);
    NetworkFound = confiWifi();
    if (!NetworkFound)
      espReset();
  }
  digitalWrite(LedPin, HIGH);

  if (InitOTA()) {
    digitalWrite(LedPin, HIGH);
#if DebugSerial
    debugPrintln(String(F("OTA OK...")));
#endif
  } else {
    digitalWrite(LedPin, LOW);
#if DebugSerial
    debugPrintln(String(F("!ERROR OTA")));
#endif
  }
  beginMdns("envoy-esp-nextion");
  NexInit(9600);
}
//////////////////////////////////////////////////////////////////////////////////////
// Principal
//////////////////////////////////////////////////////////////////////////////////////
bool Sleep = false;
uint8_t errorNetwork = 0, errorProduction = 0, errorMeter = 0, errorInverter = 0, page = 0;
uint16_t count = 60, sleepCount = 0;
void loop() {
  ArduinoOTA.handle();
  // Comprueba si la conexión WiFi NO está activa
  if (WiFi.status() != WL_CONNECTED) {
    NetworkFound = confiWifi();
    if (!NetworkFound)
      errorNetwork++;
  } else {
    errorNetwork = 0;
  }
  if (getProduction()) {
    errorProduction = 0;
    delay(10);
    if (ProdPower > 1 && Sleep == true) {
      // Despertar
      nextionSendCmd("thsp=0");
      if (ProdPower > ConPower)
        nextionSendCmd("page 1");  // A pagina 1
      else
        nextionSendCmd("page 0");  // A pagina 0
      sleepCount = 0;
      Sleep = false;
    } else if (ProdPower == 0 && Sleep == false && sleepCount == 4) {
      nextionSendCmd("page 6");  // A pagina 6-> reposo
      Sleep = true;
    }
    nextionSetAttr("strProduction.txt", String(ProdPower) + "~W~" + String(rmsCurrent) + "~A~" + String(rmsVoltage) + "~V~" + String(kWhToday) + "~Kwh~" + String(kWhLastSevenDays) + "~Kwh~" + String(InvCount));
    delay(150);
    nextionSetAttr("strConsumption.txt", String(ConPower) + "~W~" + String(ConrmsCurrent) + "~A~" + String(ConrmsVoltage) + "~V~" + String(ConkWhToday) + "~Kwh~" + String(ConkWhLastSevenDays) + "~Kwh");
    delay(150);
    nextionSetAttr("strNeta.txt", String(NetConPower) + "~W~" + ((NetConPower < 0) ? "-" : "+"));
    delay(150);
    // Obtener el tiempo actual Epoch Unix Timestamp
    time_t ahora = TimeRead;
    // Convertir el tiempo actual a una estructura de tiempo
    struct tm *tiempoActual = localtime(&ahora);
    // Crear una cadena de texto con la fecha y hora UTC
    String Time = String(tiempoActual->tm_mday) + "/" + String(tiempoActual->tm_mon + 1) + "/" + String(tiempoActual->tm_year + 1900) + "~" + String(tiempoActual->tm_hour) + ":" + String(tiempoActual->tm_min) + ":" + String(tiempoActual->tm_sec);
    nextionSetAttr("strtime.txt", Time);
    delay(250);
    if (BatCount) {
      nextionSetAttr("strstorage.txt", String(BatPower) + "~W~" + String(BatPowerWh) + "~Wh~" + String(BatStatus));
    }
    delay(250);
  } else {
    errorProduction++;
  }
  for (uint8_t i = 0; i < (Sleep == true ? 80: 40); i++) {
    delay(100);
    ArduinoOTA.handle();
  }
  page = nextionPage();
  if ((page == 2 || page == 3) && count < 50)
    count = 55;
#if DebugSerial > 1
  if (count == 6) {
#else
  if (page == 4) {
#endif
    delay(100);
    if (getMeterStream()) {
      errorMeter = 0;
      delay(10);
      nextionSetAttr("strMeterFa.txt", String(Apower) + "~W~" + String(Acurrent) + "~A~" + String(Avoltage) + "~V~" + String(Apf) + "~F~" + String(Afreq) + "~Hz");
      delay(50);
      nextionSetAttr("strMeterFb.txt", String(Bpower) + "~W~" + String(Bcurrent) + "~A~" + String(Bvoltage) + "~V~" + String(Bpf) + "~F~" + String(Bfreq) + "~Hz");
      delay(50);
      nextionSetAttr("strMeter.txt", String(PowerAB) + "~W~" + String(currentAB) + "~A~" + String(voltageAB) + "~V~" + String(ConsumptionAB) + "~w~" + String(NetPower) + "~w");
    } else {
      errorMeter++;
    }
  }
  if (++count >= 60) {
    count = 0;
    if (ProdPower < 2 && sleepCount < 4)
      sleepCount++;
    else
      sleepCount = 0;
    if (!BatCount) {
      nextionSetAttr("strstorage.txt", String(BatPower) + "~W~" + String(BatPowerWh) + "~Wh~" + String(BatStatus));
    }
    if (ProdPower < 1) {
      nextionSetAttr("strInverter.txt", "0~0~0~W~0~W~0~W");
    } else if (getInverters()) {
      errorInverter = 0;
      delay(10);
      for (uint8_t j = 0; j < InvCount; j++) {
        nextionSetAttr("strInverter.txt", String(j + 1) + "~" + String(serialNumber[j]) + "~" + String(Watts[j]) + "~W~" + String(MaxWatts[j]) + "~W~" + String(InvTotalPower) + "~W");
        delay(200);
      }
    } else {
      errorInverter++;
    }
  }
  if (count % 5 == 0) {
    if (page == 5) {
      // ip~subnet~gateWay~dns~mac~bssid~ssid~canal~rssi
      nextionSetAttr("strInfo.txt", String(WiFi.localIP().toString()) + "~" + String(WiFi.subnetMask().toString()) + "~" + String(WiFi.gatewayIP().toString()) + "~" + String(WiFi.dnsIP().toString()) + "~" + String(WiFi.macAddress()) + "~" + String(WiFi.BSSIDstr()) + "~" + String(WiFi.SSID()) + "~" + String(WiFi.channel()) + "~" + String(WiFi.RSSI()));
      delay(50);
      // Sdk~memoria~MHz~versión
      nextionSetAttr("strInfocpu.txt", String(ESP.getSdkVersion()) + "~" + String(ESP.getFreeHeap()) + "~" + String(ESP.getCpuFreqMHz()) + "~" + String(sofVersion));
    }
  }
  if ((errorNetwork + errorProduction + errorMeter + errorInverter) > 20)
    espReset();
}

//////////////////////////////////////////////////////////////////////////////////////
// Reseteamos ESP
//////////////////////////////////////////////////////////////////////////////////////
void espReset() {
#if DebugSerial
  debugPrintln(F("RESET ESP32"));
#endif
  WiFi.disconnect();
  ESP.restart();
  delay(5000);
}

#if DebugSerial
//////////////////////////////////////////////////////////////////////////////////////
// Enviamos trama de debugger
//////////////////////////////////////////////////////////////////////////////////////
void debugPrintln(String debugText) {
  yield();
  noInterrupts();
#if DebugSerial == 1
  String debugTimeText = "[+" + String(float(millis()) / 1000, 3) + "s] " + debugText;
  Serial.println(debugTimeText);
#else
  Serial.println(debugText);
#endif
  interrupts();
  yield();
  // Esperar hasta que el uart emita la interrupción TXComplete
  Serial.flush();
}
#endif
