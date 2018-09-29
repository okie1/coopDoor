//Assign the necessary I/O pins to a variable name.
const int pinSwitchOpen = D0;
const int pinSwitchClose = D1;
const int pinPhotocell = A0;
const int pinRelay1 = D3;
const int pinRelay2 = D4;


//Global enumeration that describes the current photo state.
enum PhotoState{PHOTODARK,PHOTOLIGHT};
PhotoState g_PhotoState = PhotoState::PHOTODARK;

//Global enumeration that describes the current switch state.
enum SwitchState{SWITCHOFF,SWITCHOPEN,SWITCHCLOSE};
SwitchState g_SwitchState = SwitchState::SWITCHOFF;

//Describes the current state of the coop door.
enum DoorState{DONTKNOW,OPENING,CLOSING,OPEN,CLOSED};
DoorState g_DoorState = DoorState::DONTKNOW;
String g_DoorState2 = "DontKnow";

int g_photoResistor = 0;
unsigned long g_PhotoDelayTimer = 0;
unsigned long g_PhotoDelay = 600000;//10 Minutes
bool g_IgnorePhotoCheck = true;
bool g_SwitchOverride = false;
unsigned long g_DoorTimer = 0;
unsigned long g_DoorTime = 70000;//70 seconds.
int g_LightThreshold = 100;
int g_photoTotal = 0;
int g_photoCounter = 0;
int g_SwitchOpenCounter = 0;
int g_SwitchCloseCounter = 0;



String g_Time = "";
struct FlashMemory
{
  unsigned int bootNumber;
  unsigned int lightLevel;
  char[16]     doorState;
};
FlashMemory g_FlashMemory = { 0, 0, "DontKnow" };

/*
void setup() 
{
    g_Time          = Time.format(Time.now(), "%F %T");//YYYY-MM-DD HH:MM:SS
    Particle.publish("setup");

    EEPROM.get(0,myFlashMemory);
    Particle.publish("setup1...", "Flash: boots:"+String(myFlashMemory.boots)+" lightLevel:"+String(myFlashMemory.lightLevel));
    
    myFlashMemory.boots ++;
    EEPROM.put(0,myFlashMemory);
    
    Particle.publish("setup2...", "Flash: boots:"+String(myFlashMemory.boots)+" lightLevel:"+String(myFlashMemory.lightLevel));
}

void loop() 
{
}
*/




//****************************************************************************
// Purpose - Gets a string to represent the current door state.
//****************************************************************************
void SetDoorState(DoorState desiredState)
{
    g_DoorState = desiredState;
    
    switch(g_DoorState)
    {
      case DoorState::OPENING:
      {
        g_DoorState2 = "Opening";
      }break;
      case DoorState::CLOSING:
      {
        g_DoorState2 = "Closing";
      }break;
      case DoorState::OPEN:
      {
        g_DoorState2 = "Open";
      }break;
      case DoorState::CLOSED:
      {
        g_DoorState2 = "Closed";
      }break;
      default:
      {
        g_DoorState2 = "DontKnow";
      }
  };
  
  Particle.publish("DoorState",g_DoorState2,500000,PRIVATE);
}
//****************************************************************************
//****************************************************************************



//****************************************************************************
// Purpose - Starts the process of closing the door.
//****************************************************************************
void CloseDoor()
{
  //Start time for the close.
  g_DoorTimer = millis();
  
  //Set one relay one and the other off
  //this will move extend the actuator
  digitalWrite(pinRelay1, HIGH);
  digitalWrite(pinRelay2, LOW);

  SetDoorState(DoorState::CLOSING);
}
//****************************************************************************
//****************************************************************************


//****************************************************************************
//Purpose - Starts the process of opening the door.
//****************************************************************************
void OpenDoor()
{ 
  //Start time for the open.
  g_DoorTimer = millis();

  //Set one relay off and the other on 
  //this will move retract the actuator 
  digitalWrite(pinRelay1, LOW);
  digitalWrite(pinRelay2, HIGH); 

  SetDoorState(DoorState::OPENING);
}
//****************************************************************************
//****************************************************************************

//****************************************************************************
//Purpose - Called when there is a light transition.  This will start a time
//          delay counter and save the photo state.
//****************************************************************************
void StartPhotoTimer(PhotoState photoState)
{
  //Start the time delay for the photo trip.
  g_PhotoDelayTimer = millis();

  //Save the photo state.
  g_PhotoState = photoState;

  //Allow the photo check to be executed until it is successful.
  g_IgnorePhotoCheck = false;
}
//****************************************************************************
//****************************************************************************

//****************************************************************************
//Purpose - Called each pass to see if the PhotoTimeDelay has expired.  If it
//          has, this will trigger the door to actually open.
//****************************************************************************
void CheckPhotoTimer()
{
  if(g_IgnorePhotoCheck)
    return;  

  unsigned long timeNow = millis();
  if(timeNow > g_PhotoDelayTimer + g_PhotoDelay)
  {
    Particle.publish("Photocell","ChkPhotoTmr",500000,PRIVATE);
    
	//Success! Ignore any other calls until the next light cycle.
    g_IgnorePhotoCheck = true;

    //Check to see if there was a manual override.  If there was, ignore for this cycle.
    if(g_SwitchOverride)
    {
      g_SwitchOverride = false;
      Particle.publish("Photocell","ManOverride",500000,PRIVATE);
      return;
    }
        
    Particle.publish("Photocell","MoveDoor",500000,PRIVATE);
    //The timer has expired.... Move the door!
    switch(g_PhotoState)
    {
      case PhotoState::PHOTOLIGHT:
      {
        OpenDoor();
      }break;
      case PhotoState::PHOTODARK:
      {
        CloseDoor();
      }break;
    };
  }
}
//****************************************************************************
//****************************************************************************


//****************************************************************************
//Purpose - Called to see if the door has been moving the specified amount of
//          time.  If is has, it will stop the door movement.
//****************************************************************************
void CheckDoorTimer()
{
  unsigned long timeNow = millis();
  if(timeNow > g_DoorTimer + g_DoorTime)
  {
    //The timer has expired.... Stop the door!
    
    //Set both relays off, which will stop the door.
    digitalWrite(pinRelay1, LOW);
    digitalWrite(pinRelay2, LOW); 

    //Update the status of the door state now that it has stopped.
    if(g_DoorState == DoorState::CLOSING)
      SetDoorState(DoorState::CLOSED);
    else if(g_DoorState == DoorState::OPENING)
      SetDoorState(DoorState::OPEN);
  }
}
//****************************************************************************
//****************************************************************************

void ReadPhotocell()
{
     g_photoTotal += analogRead(pinPhotocell);
     g_photoCounter++;
     
     if(g_photoCounter >= 10000)
     {
        //Particle.publish("Photocell",String(g_photoTotal),500000,PRIVATE);
        g_photoResistor =  g_photoTotal/10000;
        g_photoCounter = g_photoTotal = 0;
     }
}

bool OpenSwitch()
{
    if(digitalRead(pinSwitchOpen) == HIGH)
    {
        g_SwitchOpenCounter++;
    }
    else
    {
        g_SwitchOpenCounter = 0;
    }
    
    if(g_SwitchOpenCounter >= 20)
    {
        g_SwitchOpenCounter = 0;
        return true;
    }

    return false;
}

bool CloseSwitch()
{
    if(digitalRead(pinSwitchClose) == HIGH)
    {
        g_SwitchCloseCounter++;
    }
    else
    {
        g_SwitchCloseCounter = 0;
    }
    
    if(g_SwitchCloseCounter >= 20)
    {
        g_SwitchCloseCounter = 0;
        return true;
    }

    return false;
}

//****************************************************************************
//****************************************************************************
int moveDoor(String command)
{
    if (command=="open")
    {
        OpenDoor();
        g_SwitchState = SwitchState::SWITCHOPEN;
        g_SwitchOverride = true;
        return 1;
    }
    else if (command=="close")
    {
	    CloseDoor();
        g_SwitchState = SwitchState::SWITCHCLOSE;
        g_SwitchOverride = true;
        return 2;
    }
    else 
    {
        return -1;
    }
}
//****************************************************************************
//****************************************************************************


//****************************************************************************
//****************************************************************************
int changeLightThreshold(String command)
{
    if (command=="increase")
    {
        g_LightThreshold += 25;
        return 1;
    }
    else if (command=="decrease")
    {
        g_LightThreshold -= 25;
        return 2;
    }
    else
    {
        return -1;
    }
}
//****************************************************************************
//****************************************************************************


void setup() 
{
  //Initialized the pins for I/O.
  pinMode(pinSwitchOpen,INPUT);
  pinMode(pinSwitchClose,INPUT);
  pinMode(pinPhotocell,INPUT);
  pinMode(pinRelay1,OUTPUT);
  pinMode(pinRelay2,OUTPUT);
  
  // We are going to declare a Particle.variable() here so that we can access the value of the photoresistor from the cloud.
  Particle.variable("photoRstr", &g_photoResistor, INT);
  Particle.variable("lightThshld", &g_LightThreshold, INT);
  Particle.variable("doorState", &g_DoorState2, STRING);

  // We are also going to declare a Particle.function so that we can turn the LED on and off from the cloud.
  Particle.function("moveDoor",moveDoor);
  Particle.function("changeThshld",changeLightThreshold);
}

void loop() 
{
  //************************************************************************
  //Photo logic - move the door due to a change in light.
  {
    //Read the Photoresistor to get the current light level.
    ReadPhotocell();//g_photoResistor = analogRead(pinPhotocell);
    
    //If it was dark, but now it is light.
    if(g_PhotoState == PhotoState::PHOTODARK && g_photoResistor > g_LightThreshold)
    {
      Particle.publish("Photocell","Dark2Light",500000,PRIVATE);
	  StartPhotoTimer(PhotoState::PHOTOLIGHT);
    }
    
    //If it was light, but now it is dark
    else if(g_PhotoState == PhotoState::PHOTOLIGHT && g_photoResistor < g_LightThreshold)
    {
      Particle.publish("Photocell","Light2Dark",500000,PRIVATE);
      StartPhotoTimer(PhotoState::PHOTODARK);
    }

    //Check to see if the photo time delay has expired.  If it has, move the door.
    CheckPhotoTimer();
  }  
  //************************************************************************

  //************************************************************************
  //Switch logic - move the door due to a user request via physical switch.
  {
    if(OpenSwitch() && g_SwitchState != SwitchState::SWITCHOPEN)
    {
      Particle.publish("Swtch","Open",500000,PRIVATE);
      OpenDoor();
      g_SwitchState = SwitchState::SWITCHOPEN;
      g_SwitchOverride = true;
    }
    else if(CloseSwitch() && g_SwitchState != SwitchState::SWITCHCLOSE)
    {
      Particle.publish("Swtch","Close",500000,PRIVATE);
      CloseDoor();
      g_SwitchState = SwitchState::SWITCHCLOSE;
      g_SwitchOverride = true;
    }
    else if(digitalRead(pinSwitchOpen) == LOW && digitalRead(pinSwitchClose) == LOW)
      g_SwitchState = SwitchState::SWITCHOFF;

    //Check to see if the door timer has expired.  If it has, stop the door.
    CheckDoorTimer();
  }
  //************************************************************************
}

