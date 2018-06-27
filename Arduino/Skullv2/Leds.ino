void leds_roar()
{
  if (ledVal > 50) ledInc = -abs(ledInc);
  if (ledVal < 0) ledInc = abs(ledInc);
  ledVal += ledInc;
  leds_setAll(0, int(ledVal), 0, 0);
}
/*
void leds_set(int index, int r, int g, int b)
{
  pixels.setPixelColor(index, pixels.Color(r, g, b));
  pixels.show();
}
*/
void leds_setAll(int r, int g , int b, int t) // t for delay
{
  for (int i = 0; i < NUMLEDS; i++) pixels.setPixelColor(i, pixels.Color(r, g, b));
  pixels.show();
  delay(t);
}

void leds_blinkAll(int r, int g , int b, int nb)
{
  for (int i = 0; i < nb ; i ++)
  {
    leds_setAll(r, g, b, 50);
    leds_setAll(0, 0, 0, 50);
  }
}


void leds_flash()
{
  for (int i = 0; i < 3 ; i ++)
  {
    for (int i = 0; i < 250 ; i ++) leds_setAll(i, i, i, 1);
    for (int i = 250; i > 0 ; i --) leds_setAll(i, i, i, 1);
  }
}
