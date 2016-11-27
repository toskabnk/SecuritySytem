void inicializaGSM()
{
  Serial.println(F("Iniciando GSM..."));
  if (gsm.begin(9600)) {
    Serial.println(F("GSM Iniciado!"));
    gsmstarted = true;
    gsmpowered = true;
  }
  else
  {
    Serial.println(F("Sin respuesta... Encendiendo..."));
    gsmpower();
    delay(3000);
    if (gsm.begin(9600))
    {
      Serial.println(F("GSM Iniciado!"));
      gsmstarted = true;
      gsmpowered = true;
    }
  }
}

//Equivalencia por software a pulsar el boton de power del modulo
void gsmpower() {
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);
  delay(1000);
  digitalWrite(7, HIGH);
  delay(2000);
  digitalWrite(7, LOW);
  gsmpowered = !gsmpowered;
}

/*
 * Si hay un SMS no leido lo lee, comprueba que sea un telefono autorizado y activa o desactiva la alarma
 * TODO: Enviar el estado de la alarma si lo pide con "Estado" "estado" etc...
 */

void checkSMS()
{
  int pos;
  pos = sms.IsSMSPresent(SMS_UNREAD);

  if (pos > 0) {
    sms.GetSMS(pos, n, smsbuffer, 160);
    sms.DeleteSMS(pos);

    String smsstr = (String)smsbuffer;
    Serial.println(n);
    Serial.println(F("SMS Recibido!------------------"));
    Serial.println(smsbuffer);
    Serial.println(F("-------------------------------"));
    if ((smsstr.substring(0, 2).equals("ON") || smsstr.substring(0, 2).equals("on") || smsstr.substring(0, 2).equals("On")) && checkNum(n))
    {
      if (activarAlarma == true) {
        //sms.SendSMS(n, "Alarma ya activa");
        delay(500);
      }
      else {
        Serial.println(F("Activando alarma por sms..."));
        activaAlarma();
        delay(500);
        //sms.SendSMS(n, "Activando alarma");
      }
      intentosSMS=0;

    }
    else if ((smsstr.substring(0, 3).equals("OFF") || smsstr.substring(0, 3).equals("off") || smsstr.substring(0, 3).equals("Off")) && checkNum(n))
    {
      if (activarAlarma == false) {
        //sms.SendSMS(n, "Alarma ya desactivada");
        delay(500);
      }
      else {
        Serial.println(F("Desactivando alarma por sms..."));
        desactivaAlarma();
        delay(500);
        //sms.SendSMS(n, "Desactivando alarma");

      }
      intentosSMS=0;
    }
    else if((smsstr.substring(0, 1).equals("?") || smsstr.substring(0, 4).equals("info") || smsstr.substring(0, 4).equals("Info")) && checkNum(n)){
      if(activarAlarma){
        //sms.SendSMS(n, "La alarma se encuentra activada.");
      }
      else{
        //sms.SendSMS(n, "La alarma se encuentra desactivada.");
      }
    }
    else 
    {
      if(intentosSMS==5){
        //POR HACER
      }
    }
  }

}


//Optimizar. Comprueba si el numero esta dentro de los autorizados
bool checkNum(char* n) {
  char nu[9];
  for (int i = 0; i < numTel; i++) {
    cargaTelefonoEEPROM(nu, i);
    delay(10);
    if (nu[0] == n[3] && nu[1] == n[4] && nu[2] == n[5] && nu[3] == n[6] && nu[4] == n[7] && nu[5] == n[8] && nu[6] == n[9] && nu[7] == n[10] && nu[8] == n[11]) {
      Serial.println(F("Telefono autorizado"));
      return true;
    }
  }
  Serial.println(F("Telefono no autorizado"));
  return false;
}

//Carga en un puntero el telefono de la posicion dada.
void cargaTelefonoEEPROM(char* telefono, int pos) {
  Serial.println(F("Cargando telefono"));
  char buffer2[40];
  int i3=0;
  int i2=0;
  int i=0;
  if(pos==0){
    i2 =100 + 9;
    i = 100;
  }
  else{
    i2 = (100+(10* pos)) + 9;
    i = 100+(10* pos);
  }
  sprintf(buffer2, "Empezando desde la posicion %d", i);
  Serial.println(buffer2);
  Serial.println();
  for (i; i < i2; i++) {
    telefono[i3] = EEPROM.read(i);
    Serial.print((char)EEPROM.read(i));
    i3++;
  }
  Serial.println();
  Serial.println(F("Telefono cargado!"));
}

void resetGSM(){
  digitalWrite(8, HIGH);
  delay(1000);
  digitalWrite(8, LOW);
}

