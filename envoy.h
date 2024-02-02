#ifndef _H_ENVOY_H_
#define _H_ENVOY_H_
/*
Obtención de datos de Envoy de Enphase con versión del firmware < V7

-para firmware V7 mirar: https://github.com/manumdk/Enphase-EnvoyStreamMeter-V7-Esp32

  Basado en: 
  https://github.com/collin80/Envoy
  https://github.com/Matthew1471/Enphase-API/blob/main/Documentation/IQ%20Gateway%20API/General/Production.adoc get-production-data

  Datos obtenidos por petición a:
  http://envoy.local/production.json
  http://envoy.local/api/v1/production/inverters
  http://envoy.local/stream/meter 

 http://envoy.local/production.json
  {"production":[{"type":"inverters","activeCount":8,"readingTime":1704667569,"wNow":0,"whLifetime":7845331},
  {"type":"eim","activeCount":1,"measurementType":"production","readingTime":1704667645,"wNow":-0.0,
  "whLifetime":7909070.329,"varhLeadLifetime":0.364,"varhLagLifetime":2143363.333,"vahLifetime":9847818.739,
  "rmsCurrent":0.656,"rmsVoltage":242.95,"reactPwr":134.275,"apprntPwr":159.282,"pwrFactor":0.0,
  "whToday":12164.329,"whLastSevenDays":45423.329,"vahToday":14671.0,"varhLeadToday":0.364,"varhLagToday":3051.333}],
  "consumption":[{"type":"eim","activeCount":1,"measurementType":"total-consumption","readingTime":1704667645, 
  "wNow":272.191,"whLifetime":8472115.183,"varhLeadLifetime":6815116.786,"varhLagLifetime":2158089.52,
  "vahLifetime":13019405.509,"rmsCurrent":2.822,"rmsVoltage":242.976,"reactPwr":-519.427,"apprntPwr":685.566,
  "pwrFactor":0.4,"whToday":11208.183,"whLastSevenDays":61231.183,"vahToday":16353.0,"varhLeadToday":8910.0,
  "varhLagToday":3053.0},{"type":"eim","activeCount":1,"measurementType":"net-consumption","readingTime":1704667645,
  "wNow":272.191,"whLifetime":4140842.496,"varhLeadLifetime":6815116.422,"varhLagLifetime":14726.187,"vahLifetime":13019405.509,
  "rmsCurrent":2.166,"rmsVoltage":243.002,"reactPwr":-385.152,"apprntPwr":526.987,"pwrFactor":0.53,"whToday":0,"whLastSevenDays":0,
  "vahToday":0,"varhLeadToday":0,"varhLagToday":0}],"storage":[{"type":"acb","activeCount":0,"readingTime":0,"wNow":0,"whNow":0,"state":"idle"}]}
 
      Producción: Objeto que contiene los detalles de producción de la solicitud.
        Inversores (inverters):
           activeCount: Número de inversores activos.
              readingTime: Tiempo de lectura del dato.
              wNow: Potencia activa actual en vatios.
              whLifetime: Energía total producida durante toda la vida en vatios-hora.
     Consumo: Objeto que contiene los detalles de consumo de la solicitud.
       consumption: Representa datos de consumo de energía.
         total-consumption
          type: EIM EIM medidor de energia Envoy Integrated Meter
          activeCount: Número de dispositivos EIM activos.
          measurementType: Tipo de medición (por ejemplo, "total-consumption->consumo total (solar incluida)" o "net-consumption->consumo neto").
          readingTime: Tiempo de lectura del dato.
          wNow: Potencia activa actual en vatios.
          whLifetime: Energía total producida o producida durante toda la vida en wh.
          varhLeadLifetime: Energía reactiva en adelanto durante toda la vida útil en varh. 
          varhLagLifetime: Energía reactiva en retardo durante toda la vida útil en varh..
          vahLifetime: Representa la energía aparente acumulada en volt-amperios-hora.
          rmsCurrent: Corriente eficaz en amperios.
          rmsVoltage: Voltaje eficaz en voltios.
          reactPwr: Potencia reactiva en vatios.
          apprntPwr: Potencia aparente en vatios.
          pwrFactor: Factor de potencia.
          whToday: Energía producida hoy en vatios-hora.
          whLastSevenDays: Energía producida en los últimos siete días en vatios-hora.
          vahToday: Voltio-amperios-hora consumidos hoy.
          varhLeadToday: Energía reactiva en adelanto hoy en varh.
          varhLagToday: Energía reactiva en retardo hoy en varh.
         net-consumption
          type: EIM medidor de energia Envoy Integrated Meter.
          activeCount: Cantidad activa (número de elementos activos).
          measurementType: Tipo de medición (por ejemplo, "total-consumption->consumo total (solar incluida)" o "net-consumption->consumo neto").
          readingTime: Tiempo de lectura en formato de tiempo Unix.
          wNow: Potencia neta consumida actualmente en vatios.
          whLifetime: Energía consumida durante toda la vida útil en vatios-hora.
          varhLeadLifetime: Energía reactiva en adelanto durante toda la vida útil en varh.
          varhLagLifetime: Energía reactiva en retardo durante toda la vida útil en varh.
          vahLifetime: Voltio-amperios-hora durante toda la vida útil.
          rmsCurrent: Corriente eficaz en amperios.
          rmsVoltage: Voltaje eficaz en voltios.
          reactPwr: Potencia reactiva en vatios.
          apprntPwr: Potencia aparente en vatios.
          pwrFactor: Factor de potencia.
          whToday: Energía consumida hoy en vatios-hora.
          whLastSevenDays: Energía consumida en los últimos siete días en vatios-hora.
          vahToday: Voltio-amperios-hora consumidos hoy.
          varhLeadToday: Energía reactiva en adelanto hoy en varh.
          varhLagToday: Energía reactiva en retardo hoy en varh.
     Almacenamiento: Objeto que contiene los detalles de almacenamiento de la solicitud
       storage: Representa datos de almacenamiento de energía.
          type: Tipo ACB. baterías de AC (Encharge AC storage battery)
          activeCount: Número de ACB activos.
          readingTime: Tiempo de lectura del dato.
          wNow: Potencia activa actual en vatios.
          whNow: Energía acumulada actual en vatios-hora.
          state: Estado actual del sistema de almacenamiento 
             charging:	Cargando	La batería de CA se está cargando actualmente.
             discharging:	Descargando	La batería de CA está suministrando energía actualmente.
             full;	Llena	La batería de CA está completamente cargada.
             idle;	Inactiva	La batería de CA (si está presente) no está completamente cargada, pero tampoco está haciendo nada activamente.
 

   [{
    "serialNumber": "482xxxxxx83",
    "lastReportDate": 1705246766,
    "devType": 1,
    "lastReportWatts": 12,
    "maxReportWatts": 314
   }]
    serialNumber: Número de serie del microinversor.
    lastReportDate: Fecha de la última lectura del dispositivo en formato UNIX (marca de tiempo).
    devType: Tipo de dispositivo .
    lastReportWatts: Potencia activa reportada en la última lectura en vatios.
    maxReportWatts: Máxima potencia activa reportada por el dispositivo en vatios.

  http://envoy.local/stream/meter
    data: {“production”:{“ph-a”:{“p”:547.661,”q”:155.899,”s”:580.791,”v”:223.724,”i”:4.703,”pf”:0.94,”f”:50.0},
    ”ph-b”:{“p”:546.063,”q”:154.276,”s”:579.582,”v”:222.936,”i”:4.722,”pf”:0.94,”f”:50.0},
    ”ph-c”:{“p”:0.0,”q”:0.0,”s”:0.0,”v”:0.0,”i”:0.0,”pf”:0.0,”f”:0.0}},”net-consumption”:
    {“ph-a”:{“p”:-355.692,”q”:24.248,”s”:261.905,”v”:223.769,”i”:2.116,”pf”:-1.0,”f”:50.0},
    ”ph-b”:{“p”:36.513,”q”:31.768,”s”:87.554,”v”:222.965,”i”:0.712,”pf”:0.42,”f”:60.0},
    ”ph-c”:{“p”:0.0,”q”:0.0,”s”:0.0,”v”:0.0,”i”:0.0,”pf”:0.0,”f”:0.0}},
    ”total-consumption”:{“ph-a”:{“p”:191.969,”q”:-131.65,”s”:320.666,”v”:223.814,
    ”i”:2.587,”pf”:0.59,”f”:50.0},”ph-b”:{“p”:582.576,”q”:-122.507,”s”:668.322,
    ”v”:222.994,”i”:5.434,”pf”:0.86,”f”:50.0},”ph-c”:{“p”:0.0,”q”:0.0,”s”:0.0,
    ”v”:0.0,”i”:0.0,”pf”:0.0,”f”:0.0}}}

    Información de Producción:
        ph-a, ph-b, ph-c: Representan las tres fases.
            p: Potencia activa (en vatios).
            q: Potencia reactiva (en vars).
            s: Potencia aparente (en voltios-amperios).
            v: Voltaje (en voltios).
            i: Corriente (en amperios).
            pf: Factor de potencia.
            f: Frecuencia (en Hercios).

    Información de Consumo Neto:
        ph-a, ph-b, ph-c: Representan las tres fases.
            p: Potencia activa (en vatios), posiblemente negativa indicando inyección de energía.
            q: Potencia reactiva (en vars).
            s: Potencia aparente (en voltios-amperios).
            v: Voltaje (en voltios).
            i: Corriente (en amperios).
            pf: Factor de potencia.
            f: Frecuencia (en Hercios).

    Información de Consumo Total:
        ph-a, ph-b, ph-c: Representan las tres fases.
            p: Potencia activa (en vatios).
            q: Potencia reactiva (en vars), posiblemente negativa indicando generación de potencia reactiva.
            s: Potencia aparente (en voltios-amperios).
            v: Voltaje (en voltios).
            i: Corriente (en amperios).
            pf: Factor de potencia.
            f: Frecuencia (en Hercios).
********************************************************************************************************

ERRORES HTTPC
-1  -> HTTPC_ERROR_CONNECTION_REFUSED  Error: La conexión fue rechazada por el servidor.
-2  -> HTTPC_ERROR_SEND_HEADER_FAILED  Error: Fallo al enviar las cabeceras de la solicitud.
-3  -> HTTPC_ERROR_SEND_PAYLOAD_FAILED Error: Fallo al enviar los datos de carga útil de la solicitud.
-4  -> HTTPC_ERROR_NOT_CONNECTED       Error: La conexión no está establecida.
-5  -> HTTPC_ERROR_CONNECTION_LOST     Error: Conexión perdida durante la transmisión.
-6  -> HTTPC_ERROR_NO_STREAM           Error: No se pudo obtener el flujo de respuesta.
-7  -> HTTPC_ERROR_NO_HTTP_SERVER      Error: No se detectó un servidor HTTP en la URL especificada.
-8  -> HTTPC_ERROR_TOO_LESS_RAM        Error: No hay suficiente memoria RAM disponible.
-9  -> HTTPC_ERROR_ENCODING            Error: Problema con la codificación de datos.
-10 -> HTTPC_ERROR_STREAM_WRITE        Error: Fallo al escribir en el flujo de respuesta.
-11 -> HTTPC_ERROR_READ_TIMEOUT        Error: Expiró el tiempo de espera al leer la respuesta.

*/

#include <HTTPClient.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>

//stream/meter variables
double Apower;         // Potencia fase A
double Avoltage;       // Tensión fase A
double Acurrent;       // Corriente fase A
double Apf;            // Factor de potencia fase A
double Afreq;          // Frecuencia fase
double Bpower;         // Potencia fase B
double Bvoltage;       // Tensión fase B
double Bcurrent;       // Corriente fase B
double Bpf;            // Factor de potencia fase B
double Bfreq;          // Frecuencia fase B
double CApower;        // Potencia consumo fase A
double CBpower;        // Potencia consumo fase B
double PowerAB;        // Potencia A + B
double ConsumptionAB;  // Consumo A + B
double NetPower;       // Potencia neta
double voltageAB;      // Total tensión
double currentAB;      // Total corriente

//producción.json variables
int16_t ProdPower;           // Potencia actual en vatios.
double rmsCurrent;           // Corriente eficaz en amperios.
double rmsVoltage;           // Voltaje eficaz en voltios.
double kWhToday;             // Energía producida hoy en wh.
double kWhLastSevenDays;     // Energía producida en los últimos siete días en wh.
double WhLifetime;           // Energía total producida durante toda la vida en wh.
double MWhLifetime;          // Energía total producida durante toda la vida en Mwh.
double ConPower;             // Potencia consumida actualmente en w
double ConrmsCurrent;        // Corriente eficaz consumida en amperios.
double ConrmsVoltage;        // Voltaje eficaz consumida en voltios.
double ConkWhToday;          // Energía consumida hoy en wh.
double ConkWhLastSevenDays;  // Energía consumida en los últimos siete días en wh.
double ConWhLifetime;        // Energía total consumida durante toda la vida en wh.
double ConMWhLifetime;       // Energía total consumida durante toda la vida en Mwh.
double NetConPower;          // Potencia neta
uint8_t BatCount;            // Total baterías activas
double BatPower;             // potencia baterías
double BatPowerWh;           // potencia en kw de las baterías
String BatStatus;            // estado de las baterías charging, discharging, full, idle
int64_t TimeRead;            // tiempo en UTC de la lectura

//inversores variables
uint8_t InvCount = 0;        // Número microinversores
String serialNumber[20];     // Número de serie microinversor
int16_t Watts[20];           // Potencia microinversor
int16_t MaxWatts[20];        // Potencia pico de máxima potencia del microinversor
uint32_t InvTotalPower = 0;  // Total potencia microinversores

char envoyName[20] = "envoy";
String envoyPassword = "xxxxx";
const char* username = "installer";  //login siempre es  installer
String installerPassword = "xxxxxx";
String url = "";

// Inicia la instancia de HTTPClient
HTTPClient http;
WiFiClient client;

void debugPrintln(String);

//////////////////////////////////////////////////////////////////////////////////////
// Rutina para generar autenticación MD5 para un servidor web
// String extractParam: indica que la función  devuelve un objeto de tipo String (cadena de texto).
//////////////////////////////////////////////////////////////////////////////////////
String extractParam(String& authReq, const String& param, const char delimit) {
  // método indexOf para buscar la primera cadena "param" dentro de authReq, devuelve la posición de inicio. Si no se, devuelve -1.
  int _begin = authReq.indexOf(param);  // almacenar la posición donde se encuentra el parámetro en la cadena authReq.
  if (_begin == -1) {
    return "";
  }
  return authReq.substring(_begin + param.length(), authReq.indexOf(delimit, _begin + param.length()));
}
//////////////////////////////////////////////////////////////////////////////////////
// Función para genera resultados aleatorios (NO muy segura!!)
//////////////////////////////////////////////////////////////////////////////////////
String getCNonce(const int len) {
  static const char alphanum[] =
    "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz";
  String s = "";

  for (int i = 0; i < len; ++i) {
    s += alphanum[rand() % (sizeof(alphanum) - 1)];
  }
  return s;
}
//////////////////////////////////////////////////////////////////////////////////////
// Obtenemos trama para enviar con seguridad Digest
//////////////////////////////////////////////////////////////////////////////////////
String getDigestAuth(String& authReq, const String& username, const String& password, const String& uri, unsigned int counter) {
  // extracción de los parámetros necesarios para RFC 2069 simpler Digest
  String realm = extractParam(authReq, "realm=\"", '"');
  String nonce = extractParam(authReq, "nonce=\"", '"');
  String cNonce = getCNonce(8);

  char nc[9];
  snprintf(nc, sizeof(nc), "%08x", counter);

  // parámetros para el RFC 2617 newer Digest
  MD5Builder md5;
  md5.begin();
  md5.add(username + ":" + realm + ":" + password);  // md5 del usuario:dominio:usuario
  md5.calculate();
  String h1 = md5.toString();

  md5.begin();
  md5.add(String("GET:") + uri);
  md5.calculate();
  String h2 = md5.toString();

  md5.begin();
  md5.add(h1 + ":" + nonce + ":" + String(nc) + ":" + cNonce + ":" + "auth" + ":" + h2);
  md5.calculate();
  String response = md5.toString();

  String authorization = "Digest username=\"" + username + "\", realm=\"" + realm + "\", nonce=\"" + nonce + "\", uri=\"" + uri + "\", algorithm=\"MD5\", qop=auth, nc=" + String(nc) + ", cnonce=\"" + cNonce + "\", response=\"" + response + "\"";
#if DebugSerial > 2
  Serial.println(authorization);
#endif
  return authorization;
}
//////////////////////////////////////////////////////////////////////////////////////
// Configura el servidor MDNS
//////////////////////////////////////////////////////////////////////////////////////
bool beginMdns(String name) {
  uint8_t error = 0;
#if DebugSerial > 1
  debugPrintln(String(F("[ENVOY] Intentando iniciar MDNS con el nombre: ")) + name );
#endif
  // Nombre de host que responderá a las consultas MDNS
  if (!MDNS.begin(name)) {
#if DebugSerial > 1
    debugPrintln(String(F("[ENVOY] Nombre MDMS: ")) + name + " iniciado");
#endif
  } else {
#if DebugSerial > 1
    debugPrintln(F("[ENVOY] Error en respuesta del nombre MDNS"));
#endif
    error = 10;
  }
  IPAddress envoyIP;
  for (int i = 0; i < 4; i++) {
    envoyIP = MDNS.queryHost(envoyName, 1000);  // esperamos respuesta
    if (envoyIP > 0) {
      break;
      if (error < 5)
        error = 0;
    }
    error++;
#if DebugSerial > 1
    debugPrintln(F("[ENVOY] Fallo al obtener la IP del envoy!"));
#endif
  }
#if DebugSerial > 1
  debugPrintln(String(F("[ENVOY] IP del envoy: ")) + envoyIP.toString());
#endif
  return (error == 0);
}
//////////////////////////////////////////////////////////////////////////////////////
// Petición y obtención de datos del envoy de producción (sin contraseña)
// Obtenemos: número de inversores, potencia producida actual en w, corriente eficaz en amperios,
// voltaje eficaz en voltios, energía producida hoy en wh, energía producida en los últimos siete días en kwh;
// energía total producida durante toda la vida en wh o en Mwh.
// consumo->// Potencia consumida actualmente en w, corriente eficaz en amperios,
// voltaje eficaz en voltios, energía consumida hoy en wh, energía consumida en los últimos siete días en kwh;
// energía total consumida durante toda la vida en wh o en Mwh.
// Si disponemos de baterías, se obtiene la potencia, la energía y en que estado se encuentra
//////////////////////////////////////////////////////////////////////////////////////
bool getProduction() {
  // Especifica la URL de la solicitud
  String url = "http://envoy.local/production.json";
  DynamicJsonDocument doc(8192);  //se liberará automáticamente al final de la llamada a la función
  // Realiza la solicitud HTTP GET
  http.begin(url);
  // Almacena el código de respuesta de la solicitud
  int httpCode = http.GET();
  // Verifica si la solicitud fue exitosa
  if (httpCode > 0) {
    digitalWrite(LedPin, HIGH);
    // Recopila y muestra el contenido de la respuesta
    String payload = http.getString();
#if DebugSerial > 1
    Serial.println(F("[ENVOY] Contenido de la respuesta de production:"));
    Serial.println(payload);
#endif
    deserializeJson(doc, payload);
    JsonObject obj = doc.as<JsonObject>();
    if (obj.containsKey("production")) {
      JsonObject array0 = obj["production"][0];
      JsonObject array1 = obj["production"][1];
      if (array0["type"] == "inverters") {
        InvCount = array0["activeCount"];
#if DebugSerial > 1
        debugPrintln(String(F("[ENVOY] Número de inversores ")) + InvCount);
#endif
      }
      //Producción
      if (array1["type"] == "eim") {
        TimeRead = array1["readingTime"];   // Tiempo de lectura en formato Epoch Unix Timestamp
        ProdPower = array1["wNow"];         // Potencia de producción actual en vatios
        rmsCurrent = array1["rmsCurrent"];  // Corriente eficaz en amperios
        rmsCurrent /= 2;
        rmsVoltage = array1["rmsVoltage"];  // Voltaje eficaz en voltios
        kWhToday = array1["whToday"];       // Energía producida hoy en wh
        kWhToday /= 1000;
        kWhLastSevenDays = array1["whLastSevenDays"];  // Energía producida en los últimos siete días en kwh.
        WhLifetime = array1["whLifetime"];  // Energía total producida durante toda la vida en wh.
        if (WhLifetime > 1000000)
          MWhLifetime /= 1000000;  // Energía total producida durante toda la vida en Mwh.
#if DebugSerial
        Serial.print(F("\n-Potencia de produccion: "));
        Serial.print(ProdPower);
        Serial.print(F("w, Corriente: "));
        Serial.print(rmsCurrent);
        Serial.print(F("A, Tension: "));
        Serial.print(rmsVoltage);
        Serial.print(F("V, kWh hoy: "));
        Serial.print(kWhToday);
        Serial.print(F("kWh, 7 dias: "));
        Serial.print(kWhLastSevenDays);
        Serial.print(F("kWh, vida instalacion: "));
        if (MWhLifetime > 1) {
          Serial.print(MWhLifetime);
          Serial.println(F("MWh"));
        } else {
          Serial.print(WhLifetime);
          Serial.println(F("Wh"));
        }
#endif
      }
      // Consumo
      if (obj.containsKey("consumption")) {
        JsonObject array2 = obj["consumption"][0];
        JsonObject array3 = obj["consumption"][1];

        ConPower = array2["wNow"];             // Potencia consumida actualmente en w
        ConrmsCurrent = array2["rmsCurrent"];  // Corriente eficaz en amperios
        ConrmsCurrent /= 2;                   
        ConrmsVoltage = array2["rmsVoltage"];  // Voltaje eficaz en voltios
        ConkWhToday = array2["whToday"];       // Energía consumida hoy en wh
        ConkWhToday /= 1000;
        ConkWhLastSevenDays = array2["whLastSevenDays"];
        ConkWhLastSevenDays /= 1000;
        ConWhLifetime = array2["whLifetime"];  // Energía total consumida durante toda la vida en wh.
        if (ConWhLifetime > 1000000)
          ConMWhLifetime /= 1000000;  // Energía total consumida durante toda la vida en Mwh.
        NetConPower = array3["wNow"];
#if DebugSerial
        Serial.print("-Potencia consumida: ");
        Serial.print(ConPower);
        Serial.print("w, corriente: ");
        Serial.print(ConrmsCurrent);
        Serial.print("A, Tensión: ");
        Serial.print(ConrmsVoltage);
        Serial.print("V, kWh Hoy:");
        Serial.print(ConkWhToday);
        Serial.print("kWh, 7 días: ");
        Serial.print(ConkWhLastSevenDays);
        Serial.print("kWh, vida instalación: ");
        if (ConMWhLifetime > 1) {
          Serial.print(ConMWhLifetime);
          Serial.println(F("MWh"));
        } else {
          Serial.print(ConWhLifetime);
          Serial.println(F("Wh"));
        }
        Serial.print("-Potencia neta: ");
        Serial.print(NetConPower);
        Serial.print("kWh, kWh Hoy: ");
        Serial.print(kWhToday + ConkWhToday);
        Serial.print("kWh, 7 días: ");
        Serial.print(kWhLastSevenDays + ConkWhLastSevenDays);
        Serial.print("kWh, vida instalación: ");
        Serial.print(MWhLifetime + ConMWhLifetime);
        Serial.println("Mwh");
#endif
      }
      // Batería, "storage":[{"type":"acb","activeCount":0,"readingTime":0,"wNow":0,"whNow":0,"state":"charging-discharging-full-idle"
      BatCount = 0;
      if (obj.containsKey("storage")) {
        JsonObject array4 = obj["storage"][0];
        if (array4["type"] == "acb") {
          BatCount = array4["activeCount"];
          if (BatCount) {
            BatPower = array4["wNow"];
            BatPowerWh = array4["whNow"];
            // Obtener "state" como un const char *
            const char* state = array4["state"];
            // Convertir el estado a String
            BatStatus = String(state);
#if DebugSerial
            Serial.print("-Total baterias: ");
            Serial.print(BatCount);
            Serial.print(" Potencia: ");
            Serial.print(BatPower);
            Serial.print("W, Energia: ");
            Serial.print(BatPowerWh);
            Serial.print("Wh, Estado:");
            Serial.println(BatStatus);
            Serial.println();
#endif
          }
        }
      }
    }
  } else {
#if DebugSerial > 1
    // Imprime un mensaje de error en caso de fallo
    debugPrintln(String(F("[HTTP] Error durante la solicitud de production, código: ")) + httpCode);
#endif
    http.end();
    client.flush();
    digitalWrite(LedPin, LOW);
    return false;
  }
  // Cierra la conexión HTTP
  http.end();
  // Espera hasta que se hayan enviado todos los caracteres salientes del buffer
  client.flush();
  digitalWrite(LedPin, LOW);
  return true;
}
/////////////////////////////////////////////////////////////////////////////////////
// Petición y obtención de datos del envoy de inversores (con contraseña envoy)
/////////////////////////////////////////////////////////////////////////////////////
bool getInverters() {
  const char* uri = "/api/v1/production/inverters";
  url = "http://envoy.local/api/v1/production/inverters";
  const char* keys[] = { "WWW-Authenticate" };
  String payload = "";
  // Alrededor de 100 bytes y podríamos tener 200 de ellos por lo que 20k
  DynamicJsonDocument doc(25000);  //se liberará automáticamente al final de la llamada a la función

  http.begin(client, url);
  http.collectHeaders(keys, 1);  // Recopila las cabeceras HTTP de la respuesta y las almacena en la variable keys
  int httpCode = http.GET();
#if DebugSerial > 1
  debugPrintln(String(F("[HTTP] Respuesta-1 inverters, código: ")) + httpCode);
#endif
  // Verifica si la solicitud fue exitosa
  if (httpCode > 0) {
    String authReq = http.header("WWW-Authenticate");
    String authorization = getDigestAuth(authReq, String(username), String(installerPassword), String(uri), 1);
    http.end();
    // El proceso consta de dos pasos, acceder, reabrir y envió de autorización
    delay(200);
    http.begin(client, url);

    http.addHeader("Authorization", authorization);
    httpCode = http.GET();  // Segundo GET
#if DebugSerial > 1
    debugPrintln(String(F("[HTTP] Respuesta-2 inverters, código: ")) + httpCode);
#endif
    if (httpCode > 0) {
      digitalWrite(LedPin, HIGH);
      // Recopila contenido de la respuesta
      payload = http.getString();
#if DebugSerial > 1
      Serial.println(F("[ENVOY] Contenido de la respuesta de inverters:"));
      Serial.println(payload);
#endif
    } else {
#if DebugSerial > 1
      debugPrintln(String(F("[HTTP] Error en segunda solicitud de inverters, código: ")) + httpCode);
#endif
      http.end();
      client.flush();
      digitalWrite(LedPin, LOW);
      return false;
    }
  } else {
#if DebugSerial > 1
    debugPrintln(String(F("[HTTP] Error en primera solicitud de inverters, código: ")) + httpCode);
#endif
    http.end();
    client.flush();
    digitalWrite(LedPin, LOW);
    return false;
  }
  // Cierra la conexión HTTP
  http.end();
  deserializeJson(doc, payload);
  JsonArray obj = doc.as<JsonArray>();

  uint8_t i;
  int devType;
  uint8_t numInverters = 0;
  InvTotalPower = 0;
  for (i = 0; i < 50; i++) {
    devType = obj[i]["devType"];  // Json V7
    if (devType > 0) {
      numInverters++;
      serialNumber[i] = obj[i]["serialNumber"].as<String>();  // Número de serie microinversor
      Watts[i] = obj[i]["lastReportWatts"];                   // Potencia microinversor
      MaxWatts[i] = obj[i]["maxReportWatts"];                 // Potencia pico de máxima potencia del microinversor
      InvTotalPower += Watts[i];                              // Total potencia microinversores
    } else break;
  }
#if DebugSerial
  Serial.print("\nNúmero de inversores instalados:");
  Serial.println(i);
  numInverters = i;
  Serial.print("Potencia total microinversores: ");
  Serial.print(InvTotalPower);
  Serial.println("w");
  for (uint8_t j = 0; j < numInverters; j++) {
    Serial.print("Numero serie: ");
    Serial.print(serialNumber[j]);
    Serial.print(", Ultimo reporte: ");
    Serial.print(Watts[j]);
    Serial.print("w, Pico: ");
    Serial.print(MaxWatts[j]);
    Serial.println("w");
  }
#endif
  // Espera hasta que se hayan enviado todos los caracteres salientes del buffer.
  client.flush();
  digitalWrite(LedPin, LOW);
  return true;
}
/////////////////////////////////////////////////////////////////////////////////////
// Petición y obtención de datos del envoy del Medidor de flujo (con contraseña installer)
/////////////////////////////////////////////////////////////////////////////////////
bool getMeterStream() {
  const char* uri = "/stream/meter";
  const char* keys[] = { "WWW-Authenticate" };
  DynamicJsonDocument doc(8200);  // se liberará automáticamente al final de la llamada a la función
  String payload = "";

  url = "http://envoy.local/stream/meter";
  http.begin(client, url);
  http.collectHeaders(keys, 1);  // Recopila las cabeceras HTTP de la respuesta y las almacena en la variable keys
  int httpCode = http.GET();
#if DebugSerial > 1
  debugPrintln(String(F("[HTTP] Respuesta-1 MeterStream, código: ")) + httpCode);
#endif
  // Verifica si la solicitud fue exitosa
  if (httpCode > 0) {
    String authReq = http.header("WWW-Authenticate");
    String authorization = getDigestAuth(authReq, String(username), String(installerPassword), String(uri), 1);
    // El proceso consta de dos pasos, acceder, reabrir y envio de autorización
    http.end();
    delay(200);
    http.begin(client, url);
    http.addHeader("Authorization", authorization);
    httpCode = http.GET();
#if DebugSerial > 1
    debugPrintln(String(F("[HTTP] Respuesta-2 MeterStream, código: ")) + httpCode);
#endif
    if (httpCode > 0) {
      digitalWrite(LedPin, HIGH);
      // Recopila contenido de la respuesta
      http.setTimeout(1000); 
      payload = http.getString();
#if DebugSerial > 1
      Serial.println(F("Contenido de la respuesta de MeterStream:"));
      Serial.println(payload);
#endif
    } else {
#if DebugSerial > 1
      debugPrintln(String(F("[HTTP] Error en segunda solicitud de MeterStream, código: ")) + httpCode);
#endif
      http.end();
      client.flush();
      digitalWrite(LedPin, LOW);
      return false;
    }
  } else {
#if DebugSerial > 1
    debugPrintln(String(F("[HTTP] Error en primera solicitud de MeterStream, código: ")) + httpCode);
#endif
    http.end();
    client.flush();
    digitalWrite(LedPin, LOW);
    return false;
  }
  http.end();

  deserializeJson(doc, &payload.c_str()[5]);
  JsonObject obj = doc.as<JsonObject>();

  if (obj.containsKey("production")) {
    if (obj["production"].containsKey("ph-a")) {
      Apower = obj["production"]["ph-a"]["p"];
      Avoltage = obj["production"]["ph-a"]["v"];
      Acurrent = obj["production"]["ph-a"]["i"];
      Apf = obj["production"]["ph-a"]["pf"];
      Afreq = obj["production"]["ph-a"]["f"];
#if DebugSerial
      Serial.println("\n-Medidores-");
      Serial.print("Fase A -> ");
      Serial.print("Potencia: ");
      Serial.print(Apower);
      Serial.print("W, Tension: ");
      Serial.print(Avoltage);
      Serial.print("V, Corriente: ");
      Serial.print(Acurrent);
      Serial.print("A, Factor potencia: ");
      Serial.print(Apf);
      Serial.print("F, Frecuencia: ");
      Serial.print(Afreq);
      Serial.println("Hz");
#endif
    }
    if (obj["production"].containsKey("ph-b")) {
      Bpower = obj["production"]["ph-b"]["p"];
      Bvoltage = obj["production"]["ph-b"]["v"];
      Bcurrent = obj["production"]["ph-b"]["i"];
      Bpf = obj["production"]["ph-b"]["pf"];
      Bfreq = obj["production"]["ph-b"]["f"];
#if DebugSerial
      Serial.print("Fase B -> ");
      Serial.print("Potencia: ");
      Serial.print(Bpower);
      Serial.print("W, Tensión: ");
      Serial.print(Bvoltage);
      Serial.print("V, Corriente: ");
      Serial.print(Bcurrent);
      Serial.print("A, Factor potencia: ");
      Serial.print(Bpf);
      Serial.print("F, Frecuencia: ");
      Serial.print(Bfreq);
      Serial.println("Hz");
#endif
    }
    PowerAB = Apower + Bpower;
    voltageAB = Avoltage + Bvoltage;
    currentAB = (Acurrent + Bcurrent) / 2;
#if DebugSerial
    Serial.print("Potencia producción: ");
    Serial.print(PowerAB);
    Serial.print("W, Tensión total: ");
    Serial.print(voltageAB);
    Serial.print("V, Intensidad: ");
    Serial.print(currentAB);
    Serial.print("A, Factor potencia: ");
    Serial.print(Apf);
    Serial.print("F, Frecuencia: ");
    Serial.print(Afreq);
    Serial.println("Hz");
#endif
  }
  if (obj.containsKey("total-consumption")) {
    CApower = obj["total-consumption"]["ph-a"]["p"];
    CBpower = obj["total-consumption"]["ph-b"]["p"];
    ConsumptionAB = CApower + CBpower;
    NetPower = PowerAB + ConsumptionAB;
#if DebugSerial
    Serial.print("Potencia consumida: ");
    Serial.print(ConsumptionAB);
    Serial.print("W, potencia neta: ");
    Serial.print(NetPower);
    Serial.println("W");
    Serial.println();
#endif
  }
  client.flush();
  digitalWrite(LedPin, LOW);
  return true;
}

#endif
