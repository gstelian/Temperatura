
#include <Arduino_FreeRTOS.h>
#include <LiquidCrystal.h>
#include <semphr.h>
#include <queue.h>

LiquidCrystal lcd(12,11,5,4,3,2);
SemaphoreHandle_t x;
QueueHandle_t q;

void setup() 
{
  lcd.begin(16,2);
  Serial.begin(9600);

  xTaskCreate(Temperatura, "Temperatura", 100, NULL, 0, NULL);
  xTaskCreate(Afisare, "Afisare", 100, NULL, 2, NULL);
  xTaskCreate(Buton, "Buton", 100, NULL, 1, NULL);
  
  x=xSemaphoreCreateMutex();
  q=xQueueCreate(1,sizeof(float));
}



 char aff[5];
 float voltage;


void loop()
{
  // DO nothing
}


static void Temperatura(void* pvParameters)
{
 
   int reading = analogRead(1);
   voltage = reading * 5.0;
   voltage /= 1024.0; 
  if(! xQueueSend(q,&voltage,1000))
  {
    Serial.print("Faild to send");
  }
}


static void Afisare(void* pvParameters)
{
     if(xSemaphoreTake(x,1000))
     {
  for(int i=5;i--;i>=0)
 {
    lcd.print(aff[i]);
    if(i==1)
    lcd.print('.');
 
  Serial.print(aff[i]);
       }
     
 }
    xSemaphoreGive(x);
  
}


static void Buton(void* pvParameters){
float Volt;

  xQueueReceive(q,&Volt,1000);
  int button=analogRead(3);
    if(xSemaphoreTake(x,1000))
     {
  if(button>512)
  {
    int temperatureC = (Volt - 0.5) * 1000 ;
   
    aff[2]=temperatureC%10+0x30;
    temperatureC/=10;
    aff[1]=temperatureC%10+0x30;
    temperatureC/=10;
    aff[0]=temperatureC%10+0x30;
    temperatureC/=10;

  }
  else
  {
    int temperatureF = ((Volt - 0.5) * 900.0 / 5.0) + 32.0;

    aff[3]=temperatureF%10+0x30;
    temperatureF/=10;
    aff[2]=temperatureF%10+0x30;
    temperatureF/=10;
    aff[1]=temperatureF%10+0x30;
    temperatureF/=10;
    aff[0]=temperatureF%10+0x30;
    temperatureF/=10;
  }
       }
     xSemaphoreGive(x);
}










