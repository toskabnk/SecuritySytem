int freeRam()
{
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

//Muestra la RAM libre en la parte inferior de la pantalla.
void drawMarcoInferior() {
  //Marco Inferior
  tft.fillRect(0, 150, 128, 10, WHITE);
  tft.setCursor(10, 151);
  tft.setTextColor(RED);
  tft.setTextSize(1);
  tft.print("FREERAM:"); tft.print(freeRam());
}

//Dibuja "botones" con el texto indicado
void drawButtonn(String str1, String str2, boolean invert, boolean mainmenu) {
  //Cuadros cancelar/aceptar
  if (invert == true && mainmenu == true) {
    tft.fillRect(0, 120, 64, 30, GREEN);
    tft.fillRect(64, 120, 64, 30, BLUE);
  }
  else {
    if (invert == false && mainmenu == true) {
      tft.fillRect(0, 120, 64, 30, RED);
      tft.fillRect(64, 120, 64, 30, BLUE);
    }
    else {
      tft.fillRect(0, 120, 64, 30, GREEN);
      tft.fillRect(64, 120, 64, 30, RED);
    }
  }
  tft.setCursor(2, 125);
  tft.print(str1);
  tft.setCursor(70, 125);
  tft.print(str2);

}

//Muestra los telefonos guardados en la EEPROM
void mostrarTelefonos() {
  int k;
  int m;

  for (k = 0; k < numTel; k++) {

    Serial.print(k + 1); Serial.print(": ");

    int k2;
    int direccion = 0;

    if (k == 0) {
      direccion = 100;
    }
    else {
      direccion = 100 + (10 * k);
    }

    int direccion2 = direccion + 9;
    byte contador = 0;

    for (direccion; direccion < direccion2; direccion++) {
      Serial.print((char)EEPROM.read(direccion));
      contador++;
    }
    Serial.println();

  }
  //for(k=0;k<90;k++){
  //  Serial.print(telefono[k]);
  // }
  Serial.println(F("MOSTRADO"));
}

void warning(String s) {
  tft.fillRoundRect(10, 80, 108, 30, 8, RED);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(12, 88);
  tft.println(s);
}

//Metodo para comprobar la contraseña
//Necesita optimizacion
boolean compruebaPass(char c[]) {
  boolean res = true;
  char passIntAux[4];
  for (int auxi = 0; auxi < 4; auxi++) {
    passIntAux[auxi] = EEPROM.read(auxi + 2);
    Serial.println(EEPROM.read(auxi + 2));
  }
  for (int aux = 0; aux < 4; aux++) {
    Serial.println(passIntAux[aux]);
    Serial.println(c[aux]);
    if (passIntAux[aux] == c[aux]) {

    }
    else {
      res = false;
      break;
    }
  }
  return res;
}

//Comprueba el estado anterior de la alarma.
void estadoAnterior() {
  Serial.print(F("ESTADO ANTERIOR:"));
  if (EEPROM.read(1) == 1) {
    activarAlarma = true;
    Serial.println(F("ACTIVADA"));
  }
  else
    Serial.println(F("DESACTIVADA"));
}

void activaAlarma() {
  activarAlarma = true;
  dibujado = false;
  Serial.println(F("ACTIVANDO ALARMA"));
  if (EEPROM.read(1) != 1) {
    EEPROM.write(1, 1); //Dejamos avtivada la alarma para el estado anterior por si se apaga.
    Serial.println(F("ALARMA ACTIVADA"));
  }
  digitalWrite(13, HIGH);
}

void desactivaAlarma() {
  activarAlarma = false;
  dibujado = false;
  intrusos = false;
  avisado = false;
  movimiento = false;
  Serial.println(F("PASS CORRECTA"));
  Serial.println(F("DESACTIVANDO"));
  delay(1000);
  if (EEPROM.read(1) != 0) {
    EEPROM.write(1, 0); //Dejamos desactivada la alarma para el estado anterior por si se apaga.
  }
  digitalWrite(13, LOW);
  digitalWrite(ALARMA_SONORA, LOW);
}

void loadConfig() {


  //intentosSMS = EEPROM.read(8); delay(10);
  intentosPass = EEPROM.read(9); delay(10);
  //if (EEPROM.read(10) != 0) {
  //  avisoSMS = true;
  //}
  Serial.println(F("-----------------Config EEPROM-----------------"));
  Serial.print(F("-Activation Period: ")); Serial.println(EEPROM.read(6));
  Serial.print(F("-Grace Period: ")); Serial.println(EEPROM.read(7));
  //Serial.println(F("-Intentos SMS: ")); Serial.print(intentosSMS);
  Serial.print(F("-Intentos Pass: ")); Serial.println(intentosPass);
  Serial.println(F("-----------------------------------------------"));
}



