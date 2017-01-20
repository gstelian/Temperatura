#include <Arduino_FreeRTOS.h>
#include <semphr.h>  
#include <queue.h>
#include <LiquidCrystal.h>


LiquidCrystal lcd(12,11,5,4,3,2);
QueueHandle_t q;
SemaphoreHandle_t s;
int aff[5];
int timp=200;


void setup() {

  Serial.begin(9600);
  lcd.begin(16,2);
  
  xTaskCreate(Temperatura,  "Temperatura",  128,  NULL ,  1 ,  NULL );
  xTaskCreate(Button ,  "Button",  128 ,  NULL,  1 ,  NULL );
  xTaskCreate(Afisare ,  "Afisare",  128 ,  NULL,  1 ,  NULL );
  
  vSemaphoreCreateBinary(s);
  q=xQueueCreate(1,sizeof(float));
}

void loop()
{
  
}



void Temperatura( void *pvParameters  ) 
{
float voltage;
  for (;;)
  {
   int sensorValue = analogRead(A0);  
   voltage = sensorValue * 5.0;
   voltage /= 1024.0; 
   Serial.println("T"); 
   xQueueSend(q,&voltage,timp);
   vTaskDelay(timp);

  }
}

void Afisare( void *pvParameters  )  
{

  for (;;)
  {
    if(xSemaphoreTake(s,timp))
    {
      
    lcd.clear();
    lcd.print("Temperatura este");
    lcd.setCursor(0,1);



    
    for(int i=0;i<4;i++)
  {
    if(i==3)
    lcd.print(".");

    if(aff[0]==0 && i==0)
    continue;
   lcd.print(aff[i]);
   
  }

  if(aff[4]==1)
    {
     lcd.print(" C");
    }
    else
    {
      lcd.print(" F");
    }
    }
       Serial.println("A");
    vTaskDelay(timp);
  

}
}

void Button( void *pvParameters  )  
{
float Volt;
  for (;;)
  {
     xQueueReceive(q,&Volt,timp);
     int button=analogRead(A5);
    if(button>1020)
     {
    float tempC = (Volt - 0.5) * 100 ;
    int temperatureC=(int)(tempC*10);
    aff[4]=1;
    aff[3]=temperatureC%10;
    temperatureC/=10;
    aff[2]=temperatureC%10;
    temperatureC/=10;
    aff[1]=temperatureC%10;
    temperatureC/=10;
    aff[0]=temperatureC%10;
    temperatureC/=10;


  }
  else
  {
    float tempF = ((Volt - 0.5) * 900.0 / 5.0) + 32.0;
    int temperatureF=(int)(tempF*10);
    aff[4]=0;
    aff[3]=temperatureF%10;
    temperatureF/=10;
    aff[2]=temperatureF%10;
    temperatureF/=10;
    aff[1]=temperatureF%10;
    temperatureF/=10;
    aff[0]=temperatureF%10;
    temperatureF/=10;
  }

   Serial.println("B");
    vTaskDelay(timp);
    

    xSemaphoreGive(s);
  }
}





