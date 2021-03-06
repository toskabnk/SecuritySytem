#include <Wire.h>
#include <EEPROM.h>
#include "RF24Network.h"           //https://github.com/TMRh20/RF24Network
#include "RF24.h"                 //https://github.com/TMRh20/RF24
#include "RF24Mesh.h"            //https://github.com/TMRh20/RF24Mesh
#include "SIM900.h"
#include "sms.h"               //http://www.gsmlib.org/download/GSM_GPRS_GPS_IDE100_v307_1.zip
#include <MillisTimer.h>      //https://github.com/toskabnk/MillisTimer
#include <TFT_ILI9163C.h>    //https://github.com/sumotoy/TFT_ILI9163C
#include <Adafruit_GFX.h>   //https://github.com/adafruit/Adafruit-GFX-Library
#include <Keypad.h>        //http://playground.arduino.cc/Code/Keypad
#include <DS3232RTC.h>    //http://github.com/JChristensen/DS3232RTC
#include <Time.h>        //http://playground.arduino.cc/Code/time
#include "DHT.h"        //https://github.com/adafruit/DHT-sensor-library

#define MAXTEL 10
#define LIMPIA tft.fillScreen();
#define RESTART asm("jmp 0x0000")
#define DHTPIN 2
#define DHTTYPE DHT11
#define SENSOR 12
#define ALARMA_SONORA 5
#define CE_RF24 30
#define CSN_RF24 31


/*DIRECCIONES EEPROM
  0 Para primer inicio, puede tomar valores entre 0 y 1
  1 Para estado anterior (Activada 1, Desactivada 0)
  2 Primer digito pass
  3 Segundo digito pass
  4 Tercer digito pass
  5 Cuarto digito pass
  6 Segundos antes de activar la alarma
  7 Segundos antes de que salte a alarma al detectar un intruso
  8 Intentos SMS
  9 Intentos Pass
  10 Pendiente de aviso por sms (Por si se corta el suministro al saltar la alarma
  99 Numero de telefonos guardados
  100-109 1ºNum telefono
  110-119 2ºNum telefono
  Y asi hasta la 200
*/
//Zonas
const char zona1[] PROGMEM = "Pasillo"; //Zonas guardadas en progmem
const char zona2[] PROGMEM = "Salon";
const char zona3[] PROGMEM = "zona3";
const char* const tabla_zonas[] PROGMEM = {zona1, zona2, zona3};
char bufferZonas[10];


// strcpy_P(buffer, (char*)pgm_read_word(&(string_table[i])));


//Constantes
const byte numRows = 4; //Numero de filas y columnas  keypad
const byte numCols = 4;


//Mapeado del keypad
char keymap[numRows][numCols] =
{
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

//Conexiones a los pines del keypad
byte rowPins[numRows] = {22, 23, 24, 25}; //Filas 0 a 3
byte colPins[numCols] = {26, 27, 28, 29}; //Columnas 0 a 3

//Colores
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

//Pines Pantalla
#define __CS 10
#define __DC 9

//Variables GSM
SMSGSM sms;
boolean gsmstarted = false;
boolean gsmpowered = false;
int numdata;
char smsbuffer[160];
char n[20];

//Variables
char passInt[4]; //Pass que se introduce por el teclado
byte posInt = 0; //Posicion en la que se guarda el digito tecleado
byte numTel = EEPROM.read(99); //Numero de numeros de telefonos
byte intentosSMS = 0; //Intentos para avisar de que han intentado activar/desactivar alarma desde un contacto desconocido
byte intentosPass = 0; //Intentos
int gracePeriod = 0; //Tiempo de gracia hasta que salte la alarma
int activationPeriod = 0; //Tiempo hasta la activacion de alarma despues de conectarla

//Flags
boolean desactivar = false;
boolean activarAlarma = false;
boolean intrusos = false;
boolean dibujado = false;
boolean avisado = false;
boolean movimiento = false;
boolean configState = false;
boolean pendienteAviso = false;
boolean avisoSMS = false;
boolean bloqueado = false;
boolean cuentaAtras = false;


//Inicializamos
//DHT
DHT dht(DHTPIN, DHTTYPE);
//PANTALLA
TFT_ILI9163C tft = TFT_ILI9163C(__CS, __DC);
//TECLADO MATRICIAL
Keypad miKeypad = Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);
//RF24
RF24 radio(CE_RF24, CSN_RF24);
RF24Network network(radio);
RF24Mesh mesh(radio, network);
//TIMERS
MillisTimer activationTime(EEPROM.read(6) * 1000);//Tiempo disponible para que no te detecte la alarma al activarla
MillisTimer minuteTimer(60000); //Tiempo para la actualizacion de la hora
MillisTimer desactTimer(30000); //Tiempo para el tiempo en el menu de desactivacion
MillisTimer gracePeriodTimer(EEPROM.read(7) * 1000); //Tiempo de gracia para poder desactivar la alarma despues de detectar intrusos
MillisTimer registerCheckTimer(40000); //Tiempo para comprobar si esta registrado el modulo GSM
MillisTimer smsCheckTimer(10000); //Tiempo para comprobar si ha llegado un SMS
MillisTimer configTimer(20000); //Tiempo para salir del menu de configuracion si no se hace nada.
MillisTimer confirmacionTimer(10000); //Tiempo para salir del menu de configuracion si no se hace nada.
MillisTimer bloqueoIntentos(30000); //Tiempo que el panel esta bloqueado
MillisTimer numberTime(20000); //Tiempo hasta que se cancele la introduccion de numeros


void setup() {
  Serial.begin(9600);
  //PANTALLA
  tft.begin();
  loadLoadingScreen();
  //RF24
  mesh.setNodeID(0);
  Serial.print(F("ID Nodo:")); Serial.println(mesh.getNodeID());
  mesh.begin();
  loadingScreen(30);
  //GSM
  inicializaGSM();
  loadingScreen(120);
  //RTC
  initRTC();
  loadingScreen(160);
  //SENSOR TEMP/HUM
  dht.begin();
  loadingScreen(190);
  //LED
  pinMode(13, OUTPUT);
  loadingScreen(200);
  //Sirena
  pinMode(ALARMA_SONORA, OUTPUT);
  loadingScreen(210);
  //Pulsador
  pinMode(SENSOR, INPUT);
  loadingScreen(220);
  //Primer Inicio?
  if (EEPROM.read(0) == 0) {
    loadingScreen(255);
    Serial.println(F("Primer Inicio"));
    primerInicio(); //Si es la primera vez encedido inicia la contraseña.
    primerInicio2(); //Guarda los telefonos
    EEPROM.write(0, 1);
  }
  else {
    delay(100);
    estadoAnterior();
    loadConfig();
    loadingScreen(255);

  }

  Serial.println(F("Iniciado"));
  registerCheckTimer.setTimer();
  smsCheckTimer.setTimer();
}

void loop() {
  mesh.update();
  mesh.DHCP();
  if (network.available()) {
    RF24NetworkHeader header;
    network.peek(header);
    int idnode = 0;
    int dat2 = 0;
    uint32_t dat = 0;
    switch (header.type) {
      case 'M':
        network.read(header, &dat, sizeof(dat));
        dat2 = (int) dat;
        idnode = mesh.getNodeID(header.from_node);
        if (dat2 == 1) {
          if (idnode == 1) {
            strcpy_P(bufferZonas, (char*)pgm_read_word(&(tabla_zonas[0])));
          } else {
            strcpy_P(bufferZonas, (char*)pgm_read_word(&(tabla_zonas[1])));
          }
          if (activarAlarma) {

            movimiento = true;
          }
          Serial.print("Movimiento en zona:"); Serial.println(bufferZonas);
        } break;
      default: break;
    }
  }

  /*
     Si no se esta en el menu de desactivacion y no esta dibujado el menu lo dibuja, si esta activada uno y si no, otro.
     Si no se esta en el menu de desactivacion y esta dibujado se actualiza el tiempo y la temperatura.
  */
  if (!desactivar) {
    if (!dibujado) {
      if (configState == false) {
        if (activarAlarma) {
          mainMenu(true);
          dibujado = true;
          minuteTimer.setTimer();
        }
        else {
          mainMenu(false);
          dibujado = true;
          minuteTimer.setTimer();
        }
      }
    }
    else {
      if (minuteTimer.checkTimer()) {
        drawTimeDateTemp(true);
        minuteTimer.setTimer();
        drawMarcoInferior();
      }
    }
  }

  /*
     Menu cuando la alarma esta activada.
     Maneja la desactivacion de la alarma y el bloqueo de ella al tener 5 intentos fallidos (30segundos)
  */


  if (activarAlarma) {
    if (!desactivar) {
      char tecla2 = miKeypad.getKey();

      switch (tecla2) {
        case 'D':
          desactivar = true;
          Serial.println(F("DESACTIVAR?"));
          drawDesactMenu(); //Se dibuja el menu de desactivacion, se inicializan los temporizadores y se ubica el cursor.
          desactTimer.setTimer();
          tft.setCursor(12, 88);
          tft.setTextColor(WHITE);
          tft.setTextSize(2);
          break;

        default:
          break;
      }
    }

    if (desactivar) {

      //Si hay 5 intentos fallidos se bloquea durante 30s
      if (intentosPass == 5) {
        if (bloqueado == false) {
          bloqueado = true;
          bloqueoIntentos.setTimer();
          warning("Bloqueado");
        }

        else {
          if (bloqueoIntentos.checkTimer()) {
            bloqueado = false;
            dibujado = false;
            desactivar = false;
            intentosPass = 0;
          }
        }


      } else {
        //Si ha pasado el tiempo y no se ha introducido nada vuelve al menu.
        if (desactTimer.checkTimer()) {
          desactivar = false;
          dibujado = false;
          desactTimer.setTimer();
        }

        //Obtencion del pin de desactivacion
        else {
          char tecla3 = miKeypad.getKey();
          if (tecla3 && posInt != 4) {
            passInt[posInt] = tecla3;
            posInt++;
            tft.print("*");
            //Cuando esten los 4 digitos se comprueba se coincide la contraseña
          }
          if (posInt == 4) {
            boolean res = compruebaPass(passInt);
            if (res) {
              warning("PASS!");
              desactivaAlarma();
              desactivar = false;
              posInt = 0;
              intentosPass = 0;
              warning("PASS");
              EEPROM.write(9, 0);
            }
            else {
              desactivar = false;
              dibujado = false;
              posInt = 0;
              warning("FAIL!");
              intentosPass++;
              EEPROM.write(9, intentosPass);
              Serial.println(F("PASS INCORRECTA"));
              delay(1000);
            }
          }
        }
      }
    }

  }

  /*
     Maneja el menu de configuracion con las opciones .
     Al pasar 20 segundos sin hacer nada vuelve a la pantalla principal
  */

  else if (configState) {
    if (configTimer.checkTimer()) {
      configState = false;
      dibujado = false;
    } else {
      boolean resConfig = false;
      char tecla3 = miKeypad.getKey();
      switch (tecla3) {

        case '1':
          resConfig = confirmacion();
          if (resConfig) {
            delay(100);
            RESTART;
          }
          break;

        case'2':
          resConfig = confirmacion();
          if (resConfig) {
            EEPROM.write(99, 0);
            numTel = 0;
            primerInicio2();
          }
          drawConfig();
          break;

        case '3':
          resConfig = confirmacion();
          if (resConfig) {
            primerInicio();
          }
          drawConfig();
          break;

        case '4':
          resConfig = confirmacion();
          if (resConfig) {
            EEPROM.write(99, 0);
            numTel = 0;
          }
          drawConfig();
          break;

        case '5':
          byte auxCaptaNum;
          auxCaptaNum = capturaNumero();
          if (auxCaptaNum == 255) {

          } else {
            activationPeriod = (int)auxCaptaNum * 1000;
            EEPROM.write(6, auxCaptaNum);
            delay(10);

            Serial.println("Periodo cambiado");
            Serial.println(activationPeriod);
            Serial.println( EEPROM.read(6));
          }
          break;

        case '6':
          byte auxCaptaNum2;
          auxCaptaNum2 = capturaNumero();
          if (auxCaptaNum2 == 255) {

          } else {
            gracePeriod = (int)auxCaptaNum2 * 1000;
            EEPROM.write(7, auxCaptaNum2);
            delay(10);
            Serial.println("Periodo cambiado");
            Serial.println(gracePeriod);
            Serial.println( EEPROM.read(7));
          }
          break;

        case 'C':
          configState = false;
          dibujado = false;
          break;


        default:

          break;
      }
    }
  }

  else {
    byte yoquese;
    char tecla1 = miKeypad.getKey();
    switch (tecla1) {
      case'1':
        Serial.println(F("********Assigned Addresses********"));
        for (int i = 0; i < mesh.addrListTop; i++) {
          Serial.print("NodeID: ");
          Serial.print(mesh.addrList[i].nodeID);
          Serial.print(" RF24Network Direccion: 0");
          Serial.println(mesh.addrList[i].address, OCT);
        }
        Serial.println(F("**********************************"));
        break;

      case'2':
        Serial.println(F("********Capta Numeros********"));
        yoquese = capturaNumero();
        Serial.println(yoquese);
        Serial.println(F("**********************************"));
        break;

      case'3':
        Serial.println("CARRIER NAME");
        gsm.SimpleWriteln("AT+COPS?");
        gsm.SimpleRead();
        break;

      case 'A':
        activationTime.setTimer();
        cuentaAtras=true;
        warning("Activando");
        //activaAlarma();
        break;

      case 'C':
        drawConfig();
        dibujado = false;
        configState = true;
        configTimer.setTimer();
        break;

      case 'B':
        mostrarTelefonos(); //Para debug
        break;

      case 'D':
        //Para reiniciar el modulo GSM
        resetGSM(); //Para debug
        break;

      case '*':
        Serial.println("SIGNAL QUALITY");
        gsm.SimpleWriteln("AT+CSQ");
        gsm.SimpleRead();
        break;

      case '#':
        //Para restablecer la alarma
        EEPROM.write(99, 0);
        EEPROM.write(0, 0);
        Serial.println(F("RESTABLECIENDO DE FABRICA"));
        delay(1000);
        RESTART;
        break;

      default:

        break;
    }
  }

  if(cuentaAtras==true){
    if(activationTime.checkTimer()){
      activaAlarma();
      cuentaAtras=false;
    }
  }

  //Si la alarma esta activada, comprueba los sensores y avisa de que hay intrusos si no los habia antes
  if (activarAlarma && !intrusos) {
    if (digitalRead(SENSOR) == HIGH) {
      //Pulsador que hace de sensor (De momento)
      intrusos = true;
      gracePeriodTimer.setTimer();
      Serial.println("Intrusos!");
    }
    if (movimiento == true) {
      intrusos = true;
      gracePeriodTimer.setTimer();
      Serial.println("Intrusos!");
    }
  }

  //Si hay intrusos y no se ha avisado, envia los SMS a los telefonos registrados
  if (intrusos && !avisado) {
    if (gracePeriodTimer.checkTimer()) {
      for (int x = 0; x < numTel; x++) {

        /*
           Consigue el telefono de la EEPROM, le añade 34 (España) y envia el SMS
           Posiblemente no sea la mejor forma de añadir 34 al telefono, pero al menos funciona

           Despues de enviar los SMS enciende la sirena
        */
        char bufferTextoEnviado[40];
        sprintf(bufferTextoEnviado, "Alarma disparada en: %s", bufferZonas);
        char telAv[9];
        int telAvAux = 0;
        cargaTelefonoEEPROM(telAv, x);
        char bufferIntrusos[11];
        bufferIntrusos[0] = '3';
        bufferIntrusos[1] = '4';
        Serial.println();
        for (int p = 0; p < 9; p++) {
          Serial.print((char)telAv[p]);
        }
        Serial.println();
        for (int xx = 2; xx < 11; xx++) {
          bufferIntrusos[xx] = telAv[telAvAux];
          telAvAux++;
          Serial.print((char)bufferIntrusos[xx]);
        }
        Serial.println();

        //TODO: Identificacion de zona de la alarma disparada
        //sms.SendSMS(bufferIntrusos, "Alarma disparada!");
        for (int pp = 0; pp < 11; pp++) {
          Serial.print((char)bufferIntrusos[pp]);
        }
        Serial.println();
        Serial.print("Mensaje enviado: "); Serial.println(bufferTextoEnviado);
      }

      digitalWrite(ALARMA_SONORA, HIGH);
      avisado = true;
    }
  }

  if (avisoSMS) {
    avisoSMS = false;
    //TODO: Avisar a los numeros de los intentos fallidos
  }


  //Comprobacion de SMS
  if (smsCheckTimer.checkTimer()) {
    checkSMS();
    smsCheckTimer.setTimer();
  }



  //Si no esta regitrado o ocupado se reinicia el modulo GSM
  if (registerCheckTimer.checkTimer()) {
    if (gsm.CheckRegistration() != REG_COMM_LINE_BUSY) {
      if (gsm.CheckRegistration() != REG_REGISTERED) {
        Serial.println(F("Modulo GSM Reiniciandose..."));
        resetGSM();
      }
    }
    registerCheckTimer.setTimer();
  }


  //Fin loop
}



