
/**********************************************************************************************************
Afficheur rotatif à persistence rétinienne.   8 LEDs en rotation rapide forment des mots.
Les LEDs sont branchées aux pins 3,4,8,9,10,11,12,13
Un phototransistor branché à la pin 2 change d'état à chaque rotation.
http://electroniqueamateur.blogspot.com/2014/07/arduino-et-persistance-retinienne-pov.html
Version modifiée du sketch "THE PROPELLER DISPLAY" par Marcin Terpilowski et Babis Chatzinakis:
http://telefab.fr/2013/02/11/propeller-display/
*************************************************************************************************************/

#include "patterns.h"    //  Includes the definition of the 'charPatterns' array
#include <string.h>      //  Included for the 'strlen()' function.
#include <FastLED.h> //  Fichier qui décrit comment former chaque lettre ou nombre
#include <string.h>      //  Pour la fonction 'strlen()'

int largeur = 600;   // modifier en fonction de la vitesse du moteur:
                     // augmenter pour que le texte soit plus étalé horizontalement, 
                     // diminuer pour condenser le texte.

int dureeMot = 6666; // nombre d'itération pendant lesquelles un même texte est affiché
                     // diminuer pour que les mots changent plus vite

char * toDisplay[] = {"PERSISTENCE", "RETINIENNE","ARDUINO"};  // les mots à afficher, un après l'autre (array de pointeurs)

#define WINDOW_SIZE 16  // nombre maximal de lettres affichées à la fois

volatile char START_FLAG = 0;  //  vaut 1 quand nous sommes à l'intérieur d'un texte 


volatile unsigned char currentTextLen_g;  //  longueur du texte à afficher
volatile char * currenTextEnd_g;          //  pointeur vers le dernier caractère du texte à afficher


volatile unsigned char textOffset_g = WINDOW_SIZE * 6 - 1;  // numéro de la colonne où se trouvent présentement les LEDs


void setNextText(void){  // lorsqu'on passe au mot suivant dans la liste toDisplay
  static unsigned char currentText = 0; 
  currentTextLen_g = strlen(toDisplay[currentText]);
  currenTextEnd_g = toDisplay[currentText] + currentTextLen_g - 1;
  if(++currentText >= sizeof(toDisplay)/sizeof(toDisplay[0])){  //  retour au premier mot de la liste
    currentText = 0;
  }
}


void onInt(void){  //  réglage de la variable qui indique qu'on a atteint la position à laquelle on débute l'écriture
  START_FLAG = 1;
}

// Position des groupes de lettres dans le tableau défini dans 'patterns.c'
#define CAPITAL_LETTERS_OFFSET  0
#define SMALL_LETTERS_OFFSET  26
#define DIGITS_OFFSET (SMALL_LETTERS_OFFSET + 26)


unsigned char asciiToPatternNumber(char ascii){  // retourne la position de la lettre à l'intérieur du tableau
  if(ascii >= 'A' && ascii <= 'Z'){
    return ascii - 'A' + CAPITAL_LETTERS_OFFSET;
  }else if(ascii >= 'a' && ascii <= 'z'){
    return ascii - 'a' + SMALL_LETTERS_OFFSET;
  }else if(ascii >= '0' && ascii <= '9'){
    return ascii - '0' + DIGITS_OFFSET;
  }else{
    return DIGITS_OFFSET + 10;//space
  }
}

//  Nous déterminons l'état que doit avoir chacune des 8 LEDs en fonction du caractère à afficher,
// et la position où nous sommes à l'intérieur de ce caractère.

unsigned char getColumnPattern(unsigned char currColNo, unsigned char offset){
  unsigned char effectiveColNo = currColNo + offset;
  if(effectiveColNo >= WINDOW_SIZE * 6){
    effectiveColNo -= WINDOW_SIZE * 6;                   //  when the text goes behing the border, it reappears at the beginning
  }
  const unsigned char charOffset = effectiveColNo / 6;   //  Determine to which character of the text belongs the desired column
  if( charOffset >= currentTextLen_g ){                  //  Dont show anything behind the text that is shorter than the window
    return 0;
  }else{
    const unsigned char colOffset = effectiveColNo - charOffset * 6;   //  Calculate the column number [0..5]
    if(colOffset == 5){                                                //  Characters Separation,
      return 0;                                                        //  return empty column
    }else{
      return pgm_read_byte_near( &charPatterns[ asciiToPatternNumber( *(currenTextEnd_g - charOffset) ) * 5 + 4 - colOffset ] );
      /*
            currenTextEnd_g - charOffset    :    Pointer to the current character, calculated backwards becouse of the motor spinning direction.
            asciiToPatternNumber( *(currenTextEnd_g - charOffset) )    :    Returns the position of that character in the patterns table
            The precise number of the byte in the table is calculated by multiplication by five (the number of the columns per character).
            Since the columns flow is inverted too, the colOfset has a negative sign and is added to the value of four.
            In the end the function pgm_read_byte_near() must be used to access the table stored in the program memory.
      */
    }
  }
}


void onTimer(void){  
  static unsigned char currentColNo = 0;
  if( currentColNo < WINDOW_SIZE * 6 ){  // nous sommes à l'intérieur d'un mot
    sendToLeds(getColumnPattern(currentColNo, textOffset_g));
    currentColNo++;
  }else{  // nous ne sommes plus à l'intérieur d'un mot             
      currentColNo = 0;
      START_FLAG = 0;
      sendToLeds(0);    // on éteint les LEDs
  }
}

void setup() {
  Serial.begin(9600);
  FastLED.addLeds<WS2812, pins, GRB>(leds, nbPins);
  for(int i = 0; i < nbPins ; i++) {
    leds[i] = CRGB(0,0,0);
    FastLED.show();
  }
  Serial.println("avant l interrupt");
                     // entraînera l'exécution de onInt
  setNextText();                      // Premier mot de la liste
}



void sendToLeds(unsigned char byte){   // C'est ici qu'on commande l'allumage de la barrette de LEDs.

   if(byte & 128){
     
   }else{
      
   } 
   if(byte & 64){
     leds[6] = CRGB(0,0,0);
   FastLED.show();
   }else{
     leds[6] = CRGB(0,0,255);
   FastLED.show();
   } 
   if(byte & 32){
     leds[5] = CRGB(0,0,0);
   FastLED.show();
   }else{
     leds[5] = CRGB(255,0,2);
   FastLED.show();
   } 
   if(byte & 16){
     leds[4] = CRGB(0,0,0);
   FastLED.show();
   }else{
     leds[4] = CRGB(255,0,0);
   FastLED.show();
   } 
   if(byte & 8){
     leds[3] = CRGB(0,0,0);
   FastLED.show();
   }else{
     leds[3] = CRGB(0,255,0);
   FastLED.show();
   } 
   if(byte & 4){
     leds[2] = CRGB(0,0,0);
   FastLED.show();
   }else{
     leds[2] = CRGB(0,0,255);
   FastLED.show();
   } 
    if(byte & 2){
     leds[1] = CRGB(0,0,0);
   FastLED.show();
   }else{
     leds[1] = CRGB(255,0,255);
   FastLED.show();
   } 
   if(byte & 1){
     leds[0] = CRGB(0,0,0);
     FastLED.show();
   }else{
     leds[0] = CRGB(255,0,0);
    FastLED.show();
   } 
    
}


void loop(){
  int counter = 0; // nombre d'itérations pendant l'affichage d'un même mot

  while(1){
    if(analogRead(A5) < 200) {
      onInt();
    }
    if(START_FLAG){    //  START_FLAG est vrai quand les LEDs se trouvent à l'intérieur d'un mot
      onTimer();
      if( ++counter > dureeMot ){  
        setNextText();     //  Passage au prochain mot, si le délai prescrit est atteint
        counter = 0;
      }
      delayMicroseconds(largeur);  // on attent un peu avant la prochaine colonne
    }
  }
}
