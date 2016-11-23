
/*
 * Codigo para dibujar los menus de configuracion.
 * 
 */


void drawConfigStep1(){
     LIMPIA
     drawMarcoSuperiorConfig();
     
     //Medio
     tft.setCursor(0,22);
     tft.println("Introduce PIN nuevo  4 Digitos");

     tft.fillRoundRect(10,80,108,30,8,RED);


     drawButtonn("Acept","Canc",false,false);
     //Cuadros cancelar/aceptar *Raya en medio*
     drawMarcoInferior();
    
}

void drawConfigStep2(){
  LIMPIA
  drawMarcoSuperiorConfig();
  tft.fillRect(0,20,128,20,CYAN);
  tft.setCursor(5,23);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.println("Num. Tlf.");
  tft.setCursor(5,42);
  tft.println("Introduce el numero de telfno");
  drawButtonn("Acept","Crrg",false,false);
  drawMarcoInferior();
  tft.setCursor(5,60);
  tft.setTextSize(2);
  
  
  
}

void moreNumber(){
  tft.fillRoundRect(10,80,108,30,8,RED);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(12,88);
  tft.println("Otro tlf?");
  drawButtonn("SI","NO",false,false);
}


void drawMarcoSuperiorConfig(){
     tft.fillRect(0,0,128,20,BLUE);
     tft.setCursor(25,3);
     tft.setTextColor(WHITE);
     tft.setTextSize(2);
     tft.println("Config");
}

