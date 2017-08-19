
void configScreenUpdate() {
  tft.setCursor(0, 22);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.println("1-Reinicio");
  tft.println("2-Del Num");
  tft.println("3-Del Pass");
  tft.println("4-Del Todo");
  tft.println("5-T. Actv");
  tft.println("6-T. Alrm");
}


void drawConfig() {
  LIMPIA
  drawMarcoSuperiorConfig();
  drawMarcoInferior();
  configScreenUpdate();
}

void drawCaptaNumero() {
  LIMPIA
  drawMarcoSuperiorConfig();
  drawMarcoInferior();
  tft.setCursor(0, 22);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.println("Introduzca");
  tft.println("numero del");
  tft.println("0 al 99");
}

boolean confirmacion() {
  confirmacionTimer.setTimer();
  boolean res = false;
  boolean salirConfirmacion = true;
  warning("Seguro?");
  drawButtonn("Acept", "Canc", false, false);
  while (salirConfirmacion) {
    if (confirmacionTimer.checkTimer()) {
      salirConfirmacion = false;
      configState = false;
      dibujado = false;

    }
    char teclaConfr = miKeypad.getKey();
    switch (teclaConfr) {
      case 'A':
        res = true;
        salirConfirmacion = false;
        dibujado = false;
        break;

      case 'C':
        salirConfirmacion = false;

        break;

      default:
        break;
    }
  }
  return res;
}
byte capturaNumero() {
  boolean confirmacionAux = false;
  numberTime.setTimer();
  drawCaptaNumero();
  char c;
  unsigned int numero;
  unsigned int numero1;
  unsigned int numero2;
  boolean aceptarNumero = false;
  boolean auxNumero = false;
  boolean salirNumero = true;
  byte indiceNumero = 0;
  while (salirNumero) {
    if (numberTime.checkTimer()) {
      salirNumero = false;
      configState = false;
      dibujado = false;

    } else {
      char tecla = miKeypad.getKey();
      if (tecla) {

        if (tecla == 'A' || tecla == 'B' || tecla == 'C' || tecla == 'D' || tecla == '*' || tecla == '#') {
          //Si la tecla no es la correcta, no hagas nada
        } else {
          if (auxNumero == false) {
            tft.print(tecla);
            c = tecla;
            numero1 = c - '0';
            auxNumero = true;
          } else {
            tft.print(tecla);
            c = tecla;
            numero2 = c - '0';
            salirNumero = false;
          }
        }
      }
    }
  }
  confirmacionAux = confirmacion();
  if (confirmacionAux) {
    numero = numero1 * 10 + numero2; // num = 12
    dibujado = false;
    configState = false;
  }
  else {
    numero = 255;
  }

  return (byte)numero;
}

