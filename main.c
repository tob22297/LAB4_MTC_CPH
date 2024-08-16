#include <Arduino.h>
#include "driver/ledc.h"

//interrupciones
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

//leds
#define ledRojo 5
#define ledVerde 21
#define ledAzul 19
int z = 0;
int ledsBrillo [] = {0, 26, 51, 77, 102, 128};
int n = 0;

//PWM
#define canalPWM 1
#define canalPWMRojo 8
#define canalPWMVerde 7
#define canalPWMAzul 6

//Frecuencia PWM
#define freqPWM 60
#define freqPWMRojo 50
#define freqPWMVerde 50
#define freqPWMAzul 50

//Resolución (bits)
#define resPWM 10
#define resPWMRojo 10
#define resPWMVerde 10
#define resPWMAzul 10

#define servoMotorPin 23

int servoPosition [] = {26, 51, 77, 102, 128};
int y = 0;

// arreglos botones
struct Boton1{
  const uint8_t boton_Right;
  bool pressed;
  bool lastState;
};
Boton1 moveRight = {35, false, false};

struct Boton2{
  const uint8_t boton_Left;
  bool pressed;
  bool lastState;
};
Boton2 moveLeft = {32, false, false};

struct Boton3{
  const uint8_t boton_Color;
  bool pressed;
  bool lastState;
};
Boton3 color = {14, false, false};

struct Boton4{
  const uint8_t boton_Brillo;
  bool pressed;
  bool lastState;
};
Boton4 brillo = {13, false, false};

//variables de hadle interrupcion
void IRAM_ATTR handle_right(){
  portENTER_CRITICAL_ISR(&mux);
  moveRight.pressed = true;
  portEXIT_CRITICAL_ISR(&mux);
}

void IRAM_ATTR handle_left(){
  portENTER_CRITICAL_ISR(&mux);
  moveLeft.pressed = true;
  portEXIT_CRITICAL_ISR(&mux);
}

void IRAM_ATTR handle_color(){
  portENTER_CRITICAL_ISR(&mux);
  color.pressed = true;
  portEXIT_CRITICAL_ISR(&mux);
}

void IRAM_ATTR handle_brillo(){
  portENTER_CRITICAL_ISR(&mux);
  brillo.pressed = true;
  portEXIT_CRITICAL_ISR(&mux);
}
void initPWM(void);
void initPWMRojo(void);
void initPWMVerde(void);
void initPWMAzul(void);

void setup() {
  Serial.begin(115200);
  initPWM();  
  initPWMRojo();
  initPWMVerde();
  initPWMAzul();
  //salidas leds 
  
  //entradas botones
  pinMode(moveRight.boton_Right, INPUT_PULLUP);
  attachInterrupt(moveRight.boton_Right, handle_left, RISING);
  pinMode(moveLeft.boton_Left, INPUT_PULLUP);
  attachInterrupt(moveLeft.boton_Left, handle_right, RISING);
  pinMode(color.boton_Color, INPUT_PULLUP);
  attachInterrupt(color.boton_Color, handle_color, RISING);
  pinMode(brillo.boton_Brillo, INPUT_PULLUP);
  attachInterrupt(brillo.boton_Brillo, handle_brillo, RISING);

}
void loop() {
  if(moveRight.pressed != moveRight.lastState){
    if (moveRight.pressed) {
      y++;
      if(y > 4){
        y = 0;
      }
      moveRight.pressed = false;
    }
    moveRight.lastState = moveRight.pressed;
  }

  if(moveLeft.pressed != moveLeft.lastState){
    if (moveLeft.pressed) {
      y--;
      if(y < 0){
        y = 4;
      }
      moveLeft.pressed = false;
    } 
    moveLeft.lastState = moveLeft.pressed;
  }

  ledcWrite(canalPWM, servoPosition[y]);
  
  if(color.pressed != color.lastState) {
    if(color.pressed){
      z++;
      if(z > 3){
          z = 0;
      }
      delay(200);
      color.pressed = false;
    }
 color.lastState = color.pressed;
  }

  if(brillo.pressed != brillo.lastState){
    if(brillo.pressed){
      n++;
      if(n > 6){
        n = 0;
      }
      delay(200);
      brillo.pressed = false;
    }
    brillo.lastState = brillo.pressed;
  }

  switch(z) {
    case 0: 
      //Rojo
      ledcWrite(canalPWMRojo, ledsBrillo[n]);
      ledcWrite(canalPWMVerde, 0);
      ledcWrite(canalPWMAzul, 0);
      break;
    case 1:
      //verde
      ledcWrite(canalPWMRojo, 0);
      ledcWrite(canalPWMVerde, ledsBrillo[n]);
      ledcWrite(canalPWMAzul, 0);
      break;
    case 2:
 //azul
      ledcWrite(canalPWMRojo, 0);
      ledcWrite(canalPWMVerde, 0);
      ledcWrite(canalPWMAzul, ledsBrillo[n]);
      break;
    case 3:
      //servo
      if(y <= 1){
        ledcWrite(canalPWMRojo, ledsBrillo[n]);
        ledcWrite(canalPWMVerde, 0);
        ledcWrite(canalPWMAzul, 0);
      }
      else if(y > 1 && y <= 3){
        ledcWrite(canalPWMRojo, 0);
        ledcWrite(canalPWMVerde, ledsBrillo[n]);
        ledcWrite(canalPWMAzul, 0);
      }
      else{
        ledcWrite(canalPWMRojo, 0);
        ledcWrite(canalPWMVerde, 0);
        ledcWrite(canalPWMAzul, ledsBrillo[n]);
      }
      break;
  }
  Serial.printf("z=%u  n=%u servo=%u\n", z, n, y);
}

void initPWM(void){
  ledcSetup(canalPWM, freqPWM, resPWM);
  ledcAttachPin(servoMotorPin, canalPWM);
}
void initPWMRojo(void){
  ledcSetup(canalPWMRojo, freqPWMRojo, resPWMRojo);
  ledcAttachPin(ledRojo, canalPWMRojo);
}

void initPWMVerde(void){
  ledcSetup(canalPWMVerde, freqPWMVerde, resPWMVerde);
  ledcAttachPin(ledVerde, canalPWMVerde);
}

void initPWMAzul(void){
  ledcSetup(canalPWMAzul, freqPWMAzul, resPWMAzul);
  ledcAttachPin(ledAzul, canalPWMAzul);
}