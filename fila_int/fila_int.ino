#include <Arduino_FreeRTOS.h>
#include <FreeRTOSVariant.h>
#include <task.h>
#include <semphr.h>
#include <timers.h>

#define QUEUE_SIZE 1
#define TIMER_PERIOD pdMS_TO_TICKS(100)

const byte interruptPin = 2;
const byte outputSignalPIN = 13;

uint32_t count = 0;

void TaskCounter(void *pvParameters);

BaseType_t xTimerStarted = NULL;
SemaphoreHandle_t xSemaphoreINT0 = NULL;
QueueHandle_t xQueueData = NULL; //variável para receber a fila
TimerHandle_t xTimerSignal = NULL;

void signalGenerator (TimerHandle_t xTimerSignal); // função para o timer

void setup() {

  Serial.begin(9600);
  pinMode(interruptPin,INPUT_PULLUP);
  pinMode(outputSignalPIN,OUTPUT);

  xSemaphoreINT0 = xSemaphoreCreateBinary();

  xQueueData = xQueueCreate( QUEUE_SIZE, sizeof( uint32_t)); // cria fila para 1 dado com tamnho de dado 32 bits

  xTaskCreate(TaskCounter, (const portCHAR*)"Counter", 256, NULL, 1, NULL);
  
  xTimerSignal = xTimerCreate("Signal",100/ portTICK_PERIOD_MS ,pdTRUE,0,signalGenerator);
  
  xTimerStarted = xTimerStart(xTimerSignal,0);

  attachInterrupt(digitalPinToInterrupt(interruptPin), myInterrupt, FALLING);
}


void loop() {

}

void TaskCounter(void *pvParameters) {

  (void) pvParameters;

  uint32_t receiveData;
  for (;;) {
    xQueueReceive(xQueueData, &receiveData, portMAX_DELAY);
    Serial.println("Dado recebido:");
    Serial.println(receiveData);
  }
}

void myInterrupt(void) {

  BaseType_t xHigherPriorityTaskWoken;
  int tamanho = 0;
  xHigherPriorityTaskWoken = pdFALSE;

  uint32_t *pvDataCount;
  pvDataCount = &count;

  count++;

  tamanho = uxQueueMessagesWaitingFromISR(xQueueData);
  if (tamanho < QUEUE_SIZE)
  {
      xQueueSendToBackFromISR(xQueueData, pvDataCount, 0);
  }
 

}

void signalGenerator (TimerHandle_t xTimerSignal){
  Serial.println("Interrupt");
  if(digitalRead(outputSignalPIN) == HIGH) digitalWrite(outputSignalPIN,LOW);
  else digitalWrite(outputSignalPIN,HIGH);
  
}


