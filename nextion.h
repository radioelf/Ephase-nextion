/*
  Gestión de TX/RX datos a pantalla Nextion

  Cuando se enviá el comando 'connect' nextion responde con una cadena de datos separados por coma:
  comok 1,67-0,NX4827T043_011R,136,61488,D2670486672D4931,16777216
  comok 1->nextion con pantalla táctil
  67-0 ->reservado
  NX4827T043_011R ->modelo
  136: -> versión del Firmware
  61488 -> código MCU
  D2670486672D4931 ->Número de serie del dispositivo Nextion
  16777216 ->tamaño FLASH (byte)
  
  -------------------------------------------------------

strCommand.txt="ProdPower~1234~w~32453~kw~53.3A"

Comandos usados:
bkcmd=0 No devuelve nada
bkcmd=1 Sólo devuelve datos correctos
bkcmd=2 Sólo devuelve datos erróneos
bkcmd=3 Devuelve siempre datos
thsp=180 Periodo a SLEEP de 3 minutos
thup=1 permitimos despertar por toque en pantalla

respuesta error
24 FF FF FF -> desbordamiento del buffer serial, (máximo 1024)
*/
#ifndef NEXTION_H
#define NEXTION_H
// ESP32-S2 defecto serial1
#define RXD2 4  // GPIO RXD serial nextion
#define TXD2 5  // GPIO TXD serial nextion

byte nextionReturnBuffer[128];                       // bytes para pasar datos provenientes de Nextion
const byte nextionSuffix[3] = { 0xFF, 0xFF, 0xFF };  // Fin trama nextion

void debugPrintln(String);

//////////////////////////////////////////////////////////////////////////////////////
// UTF8-Decoder: convierte la cadena UTF8 a ASCII extendido http://playground.arduino.cc/main/Utf8ascii
// Solución para el problema aquí: https://github.com/home-assistant/home-assistant/issues/9528
// Nextion afirma que "Unicode y UTF no estarán entre las codificaciones compatibles", por lo que esto debería
// ser seguro para ejecutar contra todos los valores de atributo que entran.
// Convierte un solo personaje de UTF8 a ASCII extendido. Devuelve "0" si hay que ignorar un byte.
//////////////////////////////////////////////////////////////////////////////////////
static byte c1;
byte utf8ascii(byte ascii) {
  if (ascii < 128) {  // Conjunto estándar ASCII 0..0x7F
    c1 = 0;
    return (ascii);
  }
  // Obtener una entrada previa
  byte last = c1;  // Obtener el último carácter
  c1 = ascii;      // Recordar el carácter real
  switch (last) {  // Conversión dependiendo del primer carácter UTF8
    case 0xC2:
      return (ascii);
      break;
    case 0xC3:
      return (ascii | 0xC0);
      break;
    case 0x82:
      if (ascii == 0xAC)
        return (0x80);  // Carácter especial del símbolo del Euro
  }
  return (0);  // Si el carácter tiene que ser ignorado, retornamos cero
}

//////////////////////////////////////////////////////////////////////////////////////
// Convierte el objeto String de UTF8 String a ASCII extendido
//////////////////////////////////////////////////////////////////////////////////////
String utf8ascii(String s) {
  String r = "";
  char c;
  for (uint16_t i = 0; i < s.length(); i++) {
    c = utf8ascii(s.charAt(i));
    if (c != 0)
      r += c;
  }
  return r;
}

//////////////////////////////////////////////////////////////////////////////////////
// Conversión UTF8-string a ASCII extendido (¡ASCII más corto!)
//////////////////////////////////////////////////////////////////////////////////////
void utf8ascii(char *s) {
  uint16_t k = 0;
  char c;
  for (uint16_t i = 0; i < strlen(s); i++) {
    c = utf8ascii(s[i]);
    if (c != 0)
      s[k++] = c;
  }
  s[k] = 0;
}

//////////////////////////////////////////////////////////////////////////////////////
// Enviar el valor de un atributo del componente a Nextion
//////////////////////////////////////////////////////////////////////////////////////
void nextionSetAttr(String hmiAttribute, String hmiValue) {
  Serial1.print(hmiAttribute);
  Serial1.write(0x3D);  // =
  Serial1.write(0x22);  // "
  Serial1.print(utf8ascii(hmiValue));
  Serial1.write(0x22);  // "
  Serial1.write(nextionSuffix, 3);
#if DebugSerial > 1
  debugPrintln(String(F("[NEXTION] ")) + String(hmiAttribute) + "=" + String(hmiValue));
#endif
}

//////////////////////////////////////////////////////////////////////////////////////
// Obtener el valor de un atributo del componente Nextion
// Esto solo enviará el comando a nextion, se gestionará a través de otra función
//////////////////////////////////////////////////////////////////////////////////////
void nextionGetAttr(String hmiAttribute) {
  Serial1.print("get " + hmiAttribute);
  Serial1.write(nextionSuffix, 3);
}

//////////////////////////////////////////////////////////////////////////////////////
//Enviar un comando sin formato al panel Nextion
//////////////////////////////////////////////////////////////////////////////////////
void nextionSendCmd(String nextionCmd) {
  Serial1.print(utf8ascii(nextionCmd));
  Serial1.write(nextionSuffix, 3);
#if DebugSerial > 1
  debugPrintln(String(F("[NEXTION] ")) + String(nextionCmd));
#endif
}

//////////////////////////////////////////////////////////////////////////////////////
//  Si NO tenemos respuesta de nextion pasamos a 9600Bps para configurar velocidad a 115200Bps
//////////////////////////////////////////////////////////////////////////////////////
void nextionSetSpeed() {
  Serial1.begin(9600);
  Serial1.write(nextionSuffix, 3);
  Serial1.print("bauds=115200");  // enviamos a nextion velocidad de 115200Bps con una velocidad de 9600Bps
  Serial1.write(nextionSuffix, 3);
  Serial1.flush();
  Serial1.begin(115200);
}

//////////////////////////////////////////////////////////////////////////////////////
// Borramos buffer RX serial
//////////////////////////////////////////////////////////////////////////////////////
bool nextionclearBuffer() {
  bool Clear = false;
  yield();
  while (Serial1.available()) {
    Serial1.read();  // leemos todo el buffer
    Clear = true;
  }
  return Clear;
}

//////////////////////////////////////////////////////////////////////////////////////
// Gestionamos los datos seriales entrantes desde el panel de Nextion
// Esto recopilará datos serie del panel y los colocará en el búfer global
// nextionReturnBuffer [nextionReturnIndex]
// Retorno: verdadero si hemos recibido una cadena de 3 valores 0xFF consecutivos
// Retorno: falso si NO hemos recibido una cadena de 3 valores 0xFF consecutivos
//////////////////////////////////////////////////////////////////////////////////////
bool nextionHandleInput() {
  uint8_t nextionTermByteCnt = 0;
  uint8_t nextionReturnIndex = 0;                      // Indice de nextionReturnBuffer
  while (Serial1.available()) {
    byte nextionCommandByte = Serial1.read();
    if (nextionCommandByte == 0xFF) {   // Comprueba si tenemos uno de los 3 0xFF consecutivos que indican el final de un comando
      if (++nextionTermByteCnt == 3) {  // Hemos recibido un comando completo
        nextionReturnBuffer[nextionReturnIndex] = nextionCommandByte;
        nextionclearBuffer();
        return true;
      }
    } else {
      nextionTermByteCnt = 0;  // Reiniciamos el contador
    }
    nextionReturnBuffer[nextionReturnIndex++] = nextionCommandByte;
  }
#if DebugSerial > 1
  debugPrintln(String(F("[NEXTION] RX NO trama valida")));
#endif
  return false;
}

//////////////////////////////////////////////////////////////////////////////////////
// Reiniciar Nextion
//////////////////////////////////////////////////////////////////////////////////////
void nextionReset(int8_t Pin) {
  if (Pin == -1) {
    nextionSendCmd("rest");
    Serial1.flush();
    delay(100);
  } else {
    digitalWrite(Pin, HIGH);
    delay(250);
    digitalWrite(Pin, LOW);

    unsigned long lcdResetTimer = millis();
    while ((millis() < (lcdResetTimer + 800))) {
      yield();
    }
  }
  nextionclearBuffer();
}
//////////////////////////////////////////////////////////////////////////////////////
// Salir de modo SLEEP Nextion, con opción de un periodo de 30 segundos y acceso a pagina
//////////////////////////////////////////////////////////////////////////////////////
void nextionOffSleep(int8_t page, bool thsp) {
  if (thsp) nextionSendCmd("thsp=30");
  nextionSendCmd("sleep=0");
  if (page != -1) {
    nextionSendCmd("page " + String(page));
  }
}
//////////////////////////////////////////////////////////////////////////////////////
// Obtenemos en que pagina se encuentra Nextion
//////////////////////////////////////////////////////////////////////////////////////
uint8_t nextionPage() {
  unsigned long errorRX = 0;
  nextionclearBuffer();
  nextionSendCmd("sendme");
  while (Serial1.available() < 5) { // respuesta de 5 bytes
    delayMicroseconds(5);
    yield();
    if (errorRX++ == 5000) return NULL;  // 25ms
  }
  delay(1);
  if (nextionHandleInput()) {
    if (nextionReturnBuffer[0] == 0x66) {
#if DebugSerial > 1
      debugPrintln(String(F("[NEXTION] Pagina: ")) + String(nextionReturnBuffer[1]));
#endif
      return nextionReturnBuffer[1];
    }
  }
  return NULL; 
}
//////////////////////////////////////////////////////////////////////////////////////
// Iniciar Nextion
//////////////////////////////////////////////////////////////////////////////////////
void NexInit(int baud) {
  Serial1.begin(baud);
  nextionSendCmd("");
  nextionSendCmd("bkcmd=1");
  nextionSendCmd("thsp=0");    // Periodo a SLEEP a 0
  nextionSendCmd("thup=1");    // permitimos despetar por toque en pantalla
  nextionSendCmd("sleep=0");   // No debe pasar al moso sleep
  nextionSendCmd("page 0");    // A pagina 0
  delay(10);
#if DebugSerial > 1
  debugPrintln(String(F("[NEXTION] Serial Txd GPIO: ")) + TXD2);
  debugPrintln(String(F("[NEXTION] Serial Rxd GPIO: ")) + RXD2);
#endif
  nextionclearBuffer();
  delay(100);
}


#endif
