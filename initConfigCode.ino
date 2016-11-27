
/*
   Codigo para la configuracion inicial, pide el pin y los telefonos
   Unica parte del codigo que es bloqueante, aunque solo lo hace una vez.
*/

//Pide el pin
void primerInicio() {
  Serial.println(F("PRIMER INICIO"));
  LIMPIA
  drawConfigStep1();
  tft.setCursor(35, 95);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);

  boolean res = true; //Para salir del while
  boolean res2 = true; //Para saber si se necesita repetir este proceso
  byte k = 2; //Direccion de la EEPROM donde se empieza a guardar la pass
  byte i = 0;
  char passNueva[4]; //Pass a guardar, primero vacia.

  while (k < 6) {
    char tecla = miKeypad.getKey();
    Serial.print(tecla);
    if (tecla) {
      passNueva[i] = tecla;
      tft.print(tecla);
      i++;
      k++;
      if (tecla == 'A' || tecla == 'B' || tecla == 'C' || tecla == 'D' || tecla == '*' || tecla == '#') {
        LIMPIA
        tft.setCursor(35, 95);
        tft.println(F("No soportado"));
        tft.println(F("Solo numeros"));
        delay(2000);
        primerInicio();
      }
    }
  }

  Serial.println(F("PASS OK!"));
  Serial.println(F("GUARDAR?"));

  while (res) {
    char tecla = miKeypad.getKey();
    if (tecla == 'C') {
      Serial.println(F("GUARDANDO!"));
      res = false;
      res2 = false;
    }
    if (tecla == 'A') {
      Serial.println(F("VOLVER!"));
      res = false;
    }
  }

  if (res2 == true) {
    int k;
    for (k = 2; k < 6; k++) {
      EEPROM.write(k, passNueva[k - 2]);
      delay(5);
      Serial.println(F("OK!"));
    }
  }
  //Si le ha dado a cancelar, para atras
  if (res2 == false) {
    Serial.println(F("VOLVER!"));
    primerInicio();
  }
}



//Pedir los numeros de telefono
void primerInicio2() {
  byte telInit[9];
  boolean salir = true;
  drawConfigStep2();
  tft.setCursor(10, 95);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  pideTelefono(telInit);
  if (EEPROM.get(99) != MAXTEL) { //Comprueba si ha llegado al limite de numeros guardados
    moreNumber(); //Pregunta si quiere introducir mas telefonos
    while (salir) {
      char tecla = miKeypad.getKey();

      if (tecla == 'A') {
        salir = false;
        primerInicio2();

      }
      if (tecla == 'C') {
        Serial.println(F("NUMEROS GUARDADOS!"));
        salir = false;
      }
    }
  }
  else {
    maxNumber();
    delay(1000;)
  }

}

void pideTelefono(byte* telefono) {
  Serial.println("Numero de Telefono:");
  boolean guardar = false;
  boolean aceptar = true;
  byte i = 0;
  char numeroTelefono[9]; //Pass a guardar, primero vacia.

  while (i < 9) {
    char tecla = miKeypad.getKey();
    if (tecla) {
      if (tecla == 'A' || tecla == 'B' || tecla == 'C' || tecla == 'D' || tecla == '*' || tecla == '#') {
        //Si la tecla no es la correcta, no hagas nada
      }
      else {
        //Si es correcta, guardala.
        telefono[i] = tecla;
        tft.print(tecla);
        i++;
        Serial.print(tecla);
      }
    }
  }
  Serial.println(F(""));
  Serial.println(F("Telefono conseguido!"));
  Serial.println(F("GUARDAR?"));
  while (aceptar) {
    char tecla = miKeypad.getKey();

    if (tecla == 'A') {
      aceptar = false;
      guardar = true;
    }
    if (tecla == 'C') {
      Serial.println(F("VOLVIENDO!"));
      aceptar = false;
    }
  }

  if (guardar) {
    guardaTelefonoEEPROM(telefono);
    Serial.println(F("GUARDANDO!"));

  }
  if (!guardar) {
    primerInicio2();
  }
}

void guardaTelefonoEEPROM(byte numero[9]) {
  int i = 1;
  byte direccion = 100;

  if (numTel == 0) {
    for (i = 0; i < 9; i++) {
      //telefono[i]=numero[i];
      direccion = 100 + i;
      EEPROM.update(direccion, numero[i]);
      delay(5);
    }
  }
  else {
    int i2 = 0;
    for (i = 9 * numTel; i < ((9 * numTel) + 91); i++) {
      //telefono[i]=numero[i2];
      direccion = 100 + (10 * numTel) + i2;
      EEPROM.update(direccion, numero[i2]);
      delay(5);
      i2++;
    }
  }
  numTel++;
  EEPROM.update(99, numTel);
  delay(5);
  Serial.print("GUARDADO");
}

