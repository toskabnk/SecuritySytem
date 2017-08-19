void loadingScreen(byte porcentaje ) {
  int val = 0;
  val = map(porcentaje, 0, 255, 0, 124);
  tft.fillRect(2, 80, val, 10, WHITE);
}

void loadLoadingScreen() {
  tft.setCursor(15, 60);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.println("Cargando");
  tft.drawRect(2, 80, 124, 10, WHITE);
}

