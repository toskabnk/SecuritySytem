
/*
   Codigo para dibujar el menu de desactivacion de la pantalla
*/


void drawDesactMenu() {
  LIMPIA
  drawMarcoSuperiorDesactivar();
  tft.setCursor(0, 22);
  tft.setTextSize(2);
  tft.println("Introduzca");
  tft.println("PIN para");
  tft.println("desactivar");

  tft.fillRoundRect(10, 80, 108, 30, 8, RED);
  //tft.setCursor(12,88);

  drawButtonn("Acept", "Canc", false, false);
  drawMarcoInferior();
}


void drawMarcoSuperiorDesactivar() {
  tft.fillRect(0, 0, 128, 20, RED);
  tft.setCursor(3, 3);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.println("DESACTIVAR");
}


