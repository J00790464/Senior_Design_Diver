//Bluetooth-Serial Headers and pin definitions
#include <SoftwareSerial.h>
SoftwareSerial soft_serial(2,3);
#include <ctype.h>
//State Machine Definitions
#define REMAIN 255 //This is only to remain in the current state
#define FLOAT_STATE 0
#define SINK_STATE 1
#define SUBMERGED_STATE 2
#define RISE_STATE 3
#define COLLECT_STATE 4
#define NUMBER_OF_STATES 5

//Motor Control Pins
#define motorOne 4
#define motorTwo 5
//D4 goes to in1, D5 goes to in2, red wire goes to out1, black goes to out2
//Sensor Pins
#define MOSI 11
#define MISO 12
#define OC1A 9
//Voltage Level Pin
#define BATTERY_PIN A0
//Cutoff voltage for emergency rise
#define MOTOR_CUTOFF_VOLTAGE 6
//Motor Fill Time in ms
#define MotorFillTime 16500
<<<<<<< HEAD

//Valve servo object and pin
Servo valve_servo;
#define servo_pin 10

//User input sink timer in ms
unsigned int SinkLength;

//Base class for states
class State
{
  public:
    virtual void Enter() = 0;
    virtual uint8_t Execute() = 0;    
};

class Float_State: public State
{
  private:
    void Enter()
    {
      Serial.println("Enter Idle State");
      soft_serial.println("Type 'Dive'");
    }
  
    uint8_t Execute()
    {
      //Serial.println("Execute Idle State");
      delay(2000);
      
      String command = Get_Bt_Command();
      Serial.println(command);
 
      if (command == "Dive" || command == "DiveDive" || command == "Dive\r\n") {
        soft_serial.println("Enter length of time to dive: ");
        String timer = Get_Bt_Command();
          Serial.println(timer);
          while (timer.toInt() == 0){
            timer = Get_Bt_Command();
            Serial.println(timer);
            delay(25);
<<<<<<< HEAD
          }
      }
      //Serial.println(command);
      if (command == "Dive" || command == "DiveDive" || command == "Dive\r\n") 
      {
          soft_serial.println("Enter length of time to dive: ");
          String timer = Get_Bt_Command();
          while (timer.toInt() == 0)
          {
            timer = Get_Bt_Command();
=======
>>>>>>> parent of 1f12c66... Merge branch 'master' of https://github.com/J00790464/Senior_Design_Diver
          }
          SinkLength = timer.toInt();
          
        return SINK_STATE;
      }
      return REMAIN;
    }
String Get_Bt_Command()
    {
      while(soft_serial.available())
      {
        String bluetooth_command = soft_serial.readString();
        //bluetooth_command[bluetooth_command.length() - 2] = 0;
        return bluetooth_command;
      }
    return "N/A";
    }
};

class Sink_State: public State
{
  private:
    void Enter()
    {
      Serial.println("Enter Sink State");
    }
  
    uint8_t Execute()
    {
      Serial.println("Execute Sink State");
      digitalWrite(motorOne, HIGH);
      delay(MotorFillTime);
      digitalWrite(motorOne, LOW);
      return SUBMERGED_STATE;
    }
};
class Submerged_State:public State
{
  private:
    void Enter()
    {
      Serial.println("Enter Submerged State");
    }
    uint8_t Execute()
    {
      delay(5000);
      return COLLECT_STATE;
    }
};
class Collect_State:public State
{
  private:
    void Enter()
    {
      Serial.println("Enter Collect State");
    }
    uint8_t Execute()
    {
      delay(SinkLength);
      Serial.println(SinkLength);
      // READ SENSOR DATA***********************************************
      return RISE_STATE;
    }
};
class Rise_State:public State
{
  private:
    void Enter()
    {
      Serial.println("Enter Rise State");
    }
  
    uint8_t Execute()
    {
      Serial.println("Execute Rise State");
      digitalWrite(motorTwo, HIGH);
      delay(MotorFillTime);
      digitalWrite(motorTwo, LOW);
      return FLOAT_STATE;
    }
};


State* state_array[NUMBER_OF_STATES];
State* current_state;

long battery_check_timer = 0;
bool is_battery_low = false;

void setup() {
  Serial.begin(9600);
  soft_serial.begin(9600);
  //Initialize state array
  state_array[FLOAT_STATE] = new Float_State;
  state_array[SINK_STATE] = new Sink_State;
  state_array[SUBMERGED_STATE] = new Submerged_State;
  state_array[RISE_STATE] = new Rise_State;
  state_array[COLLECT_STATE] = new Collect_State;

  Set_State(FLOAT_STATE);
  //Initialize digital pinouts
  pinMode(motorOne, OUTPUT);
  digitalWrite(motorOne, LOW);
  pinMode(motorTwo, OUTPUT);
  digitalWrite(motorTwo,LOW);

  battery_check_timer = millis();
}

void loop() { 
  /*if(!is_battery_low)
  {
      Check_For_Low_Battery();
  }
  */
  uint8_t state = current_state->Execute();
           
  if (state != REMAIN)
  {
      Set_State(state);
  } 
}

void Set_State(uint8_t state)
{
    current_state = state_array[state];
    current_state->Enter();
}



float Get_Battery_Voltage()
{
    int battery_analog_reading = analogRead(BATTERY_PIN);
    float battery_voltage = ((float)battery_analog_reading/1024) * (5*3);
    return battery_voltage;
}

void Check_For_Low_Battery()
{
    if(millis() - battery_check_timer > 3000)
    {
       battery_check_timer = 0;
       float voltage = Get_Battery_Voltage();  
       Serial.print("Battery Voltage = ");
       Serial.println(voltage);
       if(voltage < MOTOR_CUTOFF_VOLTAGE)
       {
          is_battery_low = true;
          if(current_state != state_array[RISE_STATE])
          {
              Set_State(RISE_STATE);
          }
       }
    }
}

