#include <Arduino_FreeRTOS.h>
#include <FreeRTOSVariant.h>
#include <task.h>
#include <semphr.h>
#include <timers.h>

#define QUEUE_SIZE 1

const byte interruptPin = 2;
const byte outputSignalPIN = 13;

uint32_t count = 0;

void TaskCounter(void *pvParameters);

BaseType_t xTimerStarted = NULL;
SemaphoreHandle_t xSemaphoreINT0 = NULL;
TimerHandle_t xTimerSignal = NULL;

void signalGenerator (TimerHandle_t xTimerSignal); // função para o timer

void setup() {

  Serial.begin(9600);
  pinMode(interruptPin,INPUT_PULLUP);
  pinMode(outputSignalPIN,OUTPUT);

  xSemaphoreINT0 = xSemaphoreCreateBinary();

  xTaskCreate(TaskCounter, (const portCHAR*)"Counter", 256, NULL, 1, NULL);
  
  xTimerSignal = xTimerCreate("Signal",100/ portTICK_PERIOD_MS ,pdTRUE,0,signalGenerator);
  
  xTimerStarted = xTimerStart(xTimerSignal,0);

  attachInterrupt(digitalPinToInterrupt(interruptPin), myInterrupt, FALLING);
}


void loop() {

}

void TaskCounter(void *pvParameters) {

  (void) pvParameters;

  Serial.println("TaskCounter: iniciando task");
  for (;;) {
    xSemaphoreTake(xSemaphoreINT0, portMAX_DELAY);
    Serial.println("Task");
    Serial.println(count);
    delay(1000);
  }
}

void myInterrupt(void) {

  BaseType_t xHigherPriorityTaskWoken;
  int tamanho = 0;
  xHigherPriorityTaskWoken = pdFALSE;
  count++;
  xSemaphoreGiveFromISR(xSemaphoreINT0, &xHigherPriorityTaskWoken);

}

void signalGenerator (TimerHandle_t xTimerSignal){
  Serial.println("Interrupt");
  if(digitalRead(outputSignalPIN) == HIGH) digitalWrite(outputSignalPIN,LOW);
  else digitalWrite(outputSignalPIN,HIGH);
  
}
