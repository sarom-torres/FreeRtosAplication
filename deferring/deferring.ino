#include <Arduino_FreeRTOS.h>
#include <FreeRTOSVariant.h>
#include <task.h>
#include <semphr.h>
#include <timers.h>

const byte interruptPin0 = 2;
const byte interruptPin1 = 3;
const byte outputRed = 5;
const byte outputGreen = 7;


void TaskRedLed(void *pvParameters);
void TaskGreenLed(void *pvParameters);


SemaphoreHandle_t xSemaphoreINT0 = NULL;
SemaphoreHandle_t xSemaphoreINT1 = NULL;

void setup() {
  Serial.begin(9600);
  
  pinMode(outputRed,OUTPUT);
  pinMode(outputGreen,OUTPUT);
  
  xSemaphoreINT0 = xSemaphoreCreateBinary();
  xSemaphoreINT1 = xSemaphoreCreateBinary();
  
  xTaskCreate(TaskRedLed, (const portCHAR*)"Counter", 128, NULL, 1, NULL);
  xTaskCreate(TaskGreenLed, (const portCHAR*)"Counter", 128, NULL, 1, NULL);
  
  attachInterrupt(digitalPinToInterrupt(interruptPin0),interruptRed, HIGH);
  attachInterrupt(digitalPinToInterrupt(interruptPin1),interruptGreen, HIGH);
}

void loop() {
    
}

void TaskRedLed(void *pvParameters){
  
  (void) pvParameters;
  
  for(;;){
    xSemaphoreTake(xSemaphoreINT0, portMAX_DELAY);
    for(;;){
      delay(100);
      
      if(digitalRead(outputRed) == HIGH) digitalWrite(outputRed,LOW);
      else digitalWrite(outputRed ,HIGH);
    }
    
  }
}

void TaskGreenLed(void *pvParameters){
  
  (void) pvParameters;
  
  for(;;){
    xSemaphoreTake(xSemaphoreINT1, portMAX_DELAY);
    digitalWrite(outputRed,LOW);
    for(;;){

      delay(1000);
      if(digitalRead(outputGreen) == HIGH) digitalWrite(outputGreen,LOW);
      else digitalWrite(outputGreen,HIGH);
    }
    
  }
}

void interruptRed(void){
  
  
  BaseType_t xHigherPriorityTaskWoken;

  xHigherPriorityTaskWoken = pdFALSE;
  
  xSemaphoreGiveFromISR(xSemaphoreINT0, &xHigherPriorityTaskWoken);
  
}

void interruptGreen(void){
  
  
  BaseType_t xHigherPriorityTaskWoken;

  xHigherPriorityTaskWoken = pdFALSE;
  
  xSemaphoreGiveFromISR(xSemaphoreINT1, &xHigherPriorityTaskWoken);
  
}
