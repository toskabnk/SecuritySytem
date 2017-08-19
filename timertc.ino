void digitalClockDisplay(void)
{
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(' ');
  Serial.print(day());
  Serial.print(' ');
  Serial.print(month());
  Serial.print(' ');
  Serial.print(year());
  Serial.println();
}

void printDigits(int digits)
{
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(':');
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void initRTC() {
  setSyncProvider(RTC.get);   // Funcion para sincronizar con el RTC
  if (timeStatus() != timeSet)
    Serial.println(F("Imposible sincronizar con el RTC"));
  else
    Serial.println(F("Sincronizado con el RTC"));
}
