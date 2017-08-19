
/*
   Dibuja el menu principal, con el estado de la alarma, la fecha, la hora y la temperatura.
*/

void mainMenu(boolean res) {
  LIMPIA
  drawMarcoSuperiorMainMenu(res);
  drawCarrierName();
  drawTimeDateTemp(false);
  if (res) {
    drawButtonn("DESC", "CONF", true, true);
  }
  else {
    drawButtonn("ACTV", "CONF", false, true);
  }
  drawMarcoInferior();
}

void drawMarcoSuperiorMainMenu(boolean alm) {
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  if (alm) {
    tft.fillRect(0, 0, 128, 20, RED);
    tft.setCursor(85, 3);
    tft.println("ON");
  }
  else {
    tft.fillRect(0, 0, 128, 20, GREEN);
    tft.setCursor(90, 3);
    tft.println("OFF");
  }
  tft.setCursor(3, 3);
  tft.println("Alarma:");
}

void drawTimeDateTemp(boolean refresh) {
  tft.setTextColor(WHITE);
  if (refresh) {
    tft.fillRect(0, 25, 126, 89, BLACK);
    drawCarrierName();
    drawMarcoInferior();
    tft.setTextColor(WHITE);
  }
  tft.setCursor(25, 32);
  tft.setTextSize(3);
  //Imprimir hora
  if (hour() < 10) {
    tft.print("0"); //Para añadir un 0 al mes si es menor que 10
  }
  tft.print(hour());

  tft.print(":");

  if (minute() < 10) {
    tft.print("0"); //Para añadir un 0 al minuto si es menor que 10
  }
  tft.println(minute());

  //Imprimir Temp
  tft.setCursor(30, 80);
  tft.print((int)dht.readTemperature()); tft.print((char)167); tft.println("C");

  //Imprimir dia y mes
  tft.setTextSize(2);
  tft.setCursor(3, 60);
  if (day() < 10) {
    tft.print("0"); //Para añadir un 0 al dia si es menor que 10
  }
  tft.print(day()); tft.print("/"); tft.print(month()); tft.print("/"); tft.println(year());



}

void drawCarrierName() {
  tft.setCursor(52, 22);
  tft.setTextSize(1);

  tft.println("SYMIO"); //TODO: Conseguirlo de la SIM
}

void drawSignalBars() {
  //TODO: Por hacer
}

