// 433 MHz Přijímač

// připojení knihovny
#include <VirtualWire.h>
#include <PWMServo.h>

PWMServo rizeni;

const int acc_zero = 127;

const int motorPin = 12;

void setup()
{
  // inicializace komunikace po sériové lince
  Serial.begin(9600);
  // nastavení typu bezdrátové komunikace
  vw_set_ptt_inverted(true);
  // nastavení rychlosti přenosu v bitech za sekundu
  vw_setup(1000);
  // nastavení čísla datového pinu pro přijímač
  vw_set_rx_pin(7);
  // nastartování komunikace po nastaveném pinu
  vw_rx_start();

  pinMode(motorPin, OUTPUT);
  rizeni.attach(9);
}

void accelerate (int hodnota)
{
  if(hodnota > (acc_zero + 5)){   //jedeme dopredu
   int rychlost = hodnota - acc_zero;
   analogWrite(motorPin, rychlost);

// } 
//  else if(hodnota > (acc_zero - 5)){   //jedeme dozadu

  }
  else {      //stojime
    analogWrite(motorPin, 0);
  }
  
}

void steering (int hodnota)
{
  int uhel = map(hodnota, 0, 1023, 0, 180);     // premapovani na uhel serva (value between 0 and 180)
  rizeni.write(uhel);                  // nastaveni uhli
  delay(15);                           // pockani na pootoceni serva
}

void loop() {
  // vytvoření proměnných pro uložení
  // přijaté zprávy a její délky,
  // délka je maximálně 78 znaků
  uint8_t zprava[VW_MAX_MESSAGE_LEN];
  uint8_t delkaZpravy = VW_MAX_MESSAGE_LEN;
  // v případě přijetí zprávy se vykoná tato if funkce
  if (vw_get_message(zprava, &delkaZpravy)) {
    // rozsvícení LED diody při příjmu (nepovinné)
    digitalWrite(13, true);
    // vytištění celé zprávy po znacích
    // pomocí for cyklu
    for (int i = 0; i < delkaZpravy; i++) {
      Serial.print((char)zprava[i]);
    }
    // ukončení vypsaného řádku pomocí println
    Serial.println("");
    // zhasnutí LED diody při příjmu (nepovinné)
    digitalWrite(13, false);

    //Vyhodnotit kteremu kanalu zprava patri a vycist hodnotu a prevest na int
    int hodnota;
    
    switch(delkaZpravy){
      case 2: 
         hodnota = zprava[1];
         break;
      case 3:
        hodnota = (10 * zprava[1]) + zprava[2];
        break;
      case 4:
        hodnota = (100* zprava[1]) + (10 * zprava[2]) + zprava[3];
        break;
    }
    if (zprava[0] == 'A') {
       accelerate(hodnota);
    }
        if (zprava[0] == 'B') {
       steering(hodnota);
    }
  }
}
