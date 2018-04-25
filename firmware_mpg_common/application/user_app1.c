/**********************************************************************************************************************
File: user_app.c                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app as a template:
 1. Copy both user_app.c and user_app.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

Description:
This is a user_app.c file template 

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:


Protected System functions:
void UserApp1Initialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.

void UserApp1RunActiveState(void)
Runs current task state.  Should only be called once in main loop.


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                       /* Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern u32 G_u32AntApiCurrentDataTimeStamp;                       /* From ant_api.c */
extern AntApplicationMessageType G_eAntApiCurrentMessageClass;    /* From ant_api.c */
extern u8 G_au8AntApiCurrentMessageBytes[ANT_APPLICATION_MESSAGE_BYTES];  /* From ant_api.c */
extern AntExtendedDataType G_sAntApiCurrentMessageExtData;        /* From ant_api.c */

extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */



/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static u32 UserApp1_u32DataMsgCount = 0;             /* Counts the number of ANT_DATA packets received */
static u32 UserApp1_u32TickMsgCount = 0;             /* Counts the number of ANT_TICK packets received */

static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */
static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */
static u8 u8Role = 1;
static AntAssignChannelInfoType sAntSetupData;

/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Function: UserApp1Initialize

Description:
Initializes the State Machine and its variables.

Requires:
  -

Promises:
  - 
*/
void UserApp1Initialize(void)
{
  u8 au8WelcomeMessage[] = "Hide and Go seek";
  u8 au8Instructions[] = "Press B0 to Start";
  
  /* Clear screen and place start messages */
  LCDCommand(LCD_CLEAR_CMD);
  LCDMessage(LINE1_START_ADDR, au8WelcomeMessage); 
  LCDMessage(LINE2_START_ADDR, au8Instructions); 


  
 /* Configure ANT for this application */
  sAntSetupData.AntChannel          = ANT_CHANNEL_USERAPP;
  sAntSetupData.AntChannelType      = ANT_CHANNEL_TYPE_USERAPP1;
  sAntSetupData.AntChannelPeriodLo  = ANT_CHANNEL_PERIOD_LO_USERAPP;
  sAntSetupData.AntChannelPeriodHi  = ANT_CHANNEL_PERIOD_HI_USERAPP;
  
  sAntSetupData.AntDeviceIdLo       = ANT_DEVICEID_LO_USERAPP;
  sAntSetupData.AntDeviceIdHi       = ANT_DEVICEID_HI_USERAPP;
  sAntSetupData.AntDeviceType       = ANT_DEVICE_TYPE_USERAPP;
  sAntSetupData.AntTransmissionType = ANT_TRANSMISSION_TYPE_USERAPP;
  sAntSetupData.AntFrequency        = ANT_FREQUENCY_USERAPP;
  sAntSetupData.AntTxPower          = ANT_TX_POWER_USERAPP;

  sAntSetupData.AntNetwork = ANT_NETWORK_DEFAULT;
  for(u8 i = 0; i < ANT_NETWORK_NUMBER_BYTES; i++)
  {
    sAntSetupData.AntNetworkKey[i] = ANT_DEFAULT_NETWORK_KEY;
  }
    
  /* If good initialization, set state to Idle */
  if( AntAssignChannel(&sAntSetupData) )
  {
    UserApp1_StateMachine = UserApp1SM_WaitChannelAssign;
  }
  else
  {
    UserApp1_StateMachine = UserApp1SM_Error;
  }

} /* end UserApp1Initialize() */


/*----------------------------------------------------------------------------------------------------------------------
Function UserApp1RunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.
All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
  - State machine function pointer points at current state

Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void UserApp1RunActiveState(void)
{
  UserApp1_StateMachine();

} /* end UserApp1RunActiveState */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for the ANT channel assignment to finish */
static void UserApp1SM_WaitChannelAssign(void)
{
  /* Check if the channel assignment is complete */
  if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_CONFIGURED)
  {
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  
  /* Monitor for timeout */
  if( IsTimeUp(&UserApp1_u32Timeout, 3000) )
  {
    DebugPrintf("\n\r***Channel assignment timeout***\n\n\r");
    UserApp1_StateMachine = UserApp1SM_Error;
  }
      
} /* end UserApp1SM_WaitChannelAssign() */

static void UserApp1SM_Choose(void)
{
  if(u8Role==1)
  {
    sAntSetupData.AntChannelType      = ANT_CHANNEL_TYPE_USERAPP1;
    if( AntAssignChannel(&sAntSetupData) )
    {
      UserApp1_StateMachine = UserApp1SM_Open;
    }
    else
    {
      UserApp1_StateMachine = UserApp1SM_Error;
    }
  }
  if(u8Role==2)
  {
    sAntSetupData.AntChannelType      = ANT_CHANNEL_TYPE_USERAPP2;
    if( AntAssignChannel(&sAntSetupData) )
    {
      UserApp1_StateMachine = UserApp1SM_Open;
    }
    else
    {
      UserApp1_StateMachine = UserApp1SM_Error;
    }
  }
}

static void UserApp1SM_Open(void)
{
  if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_CONFIGURED)
  {
    AntOpenChannelNumber(ANT_CHANNEL_USERAPP);
    UserApp1_StateMachine = UserApp1SM_WaitTenSeconds;
  }
}

static void UserApp1SM_WaitTenSeconds(void)
{
  u8 au8WaitMessage[]="Please Wait 10s";
  u8 au8SeekerWaitMessage[]="Ready or not";
  u8 au8SeekerWaitMessage1[]="Here I come";
  u8 au8RoleName1[]="Seeker";
  u8 au8RoleName2[]="Hider";
  static u32 u32Counter = 0;
  
  if(u32Counter==0)
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, au8WaitMessage);
    if(u8Role==1)
    {
      LCDMessage(LINE2_START_ADDR, au8SeekerWaitMessage);
    }
  }
  u32Counter++;
  
  if(u32Counter>10000)
  {
    if(u8Role==1)
    {
      LCDCommand(LCD_CLEAR_CMD);
      LCDMessage(LINE1_START_ADDR, au8RoleName1);
      LCDMessage(LINE2_START_ADDR, au8SeekerWaitMessage1);
      UserApp1_StateMachine = UserApp1SM_Seek;
    }
    if(u8Role==2)
    {
      LCDCommand(LCD_CLEAR_CMD);
      LCDMessage(LINE1_START_ADDR, au8RoleName2);
      UserApp1_StateMachine = UserApp1SM_Hide;
    }
  }
  
}

static void UserApp1SM_Seek(void)
{
  u8 au8SeekMessage1[]="Finding";
  u8 au8SeekMessage2[]="Found you";
  static u8 au8FoundMessage[] = {1,1,1,1,1,1,1,1};
  static s8 s8Rssi = -100;
  
  
  if( AntReadAppMessageBuffer() )
  {
    if(G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      s8Rssi=G_sAntApiCurrentMessageExtData.s8RSSI;
      if(s8Rssi<= -100)
      {
        LCDCommand(LCD_CLEAR_CMD);
        LCDMessage(LINE2_START_ADDR+13, au8SeekMessage1);
        LedOn(WHITE);
        LedOff(PURPLE);    
        LedOff(BLUE);    
        LedOff(CYAN);    
        LedOff(GREEN);    
        LedOff(YELLOW);    
        LedOff(ORANGE);    
        LedOff(RED); 
      }
      if(s8Rssi> -100 && s8Rssi<=-92)
      {
        LCDCommand(LCD_CLEAR_CMD);
        LCDMessage(LINE2_START_ADDR+13, au8SeekMessage1);
        LedOn(WHITE);
        LedOn(PURPLE);    
        LedOff(BLUE);    
        LedOff(CYAN);    
        LedOff(GREEN);    
        LedOff(YELLOW);    
        LedOff(ORANGE);    
        LedOff(RED); 
      }
      if(s8Rssi> -92 && s8Rssi<=-84)
      {
        LCDCommand(LCD_CLEAR_CMD);
        LCDMessage(LINE2_START_ADDR+13, au8SeekMessage1);
        LedOn(WHITE);
        LedOn(PURPLE);    
        LedOn(BLUE);    
        LedOff(CYAN);    
        LedOff(GREEN);    
        LedOff(YELLOW);    
        LedOff(ORANGE);    
        LedOff(RED); 
      }
      if(s8Rssi> -84 && s8Rssi<=-76)
      {
        LCDCommand(LCD_CLEAR_CMD);
        LCDMessage(LINE2_START_ADDR+13, au8SeekMessage1);
        LedOn(WHITE);
        LedOn(PURPLE);    
        LedOn(BLUE);    
        LedOn(CYAN);    
        LedOff(GREEN);    
        LedOff(YELLOW);    
        LedOff(ORANGE);    
        LedOff(RED); 
      }
      if(s8Rssi> -76 && s8Rssi<=-68)
      {
        LCDCommand(LCD_CLEAR_CMD);
        LCDMessage(LINE2_START_ADDR+13, au8SeekMessage1);
        LedOn(WHITE);
        LedOn(PURPLE);    
        LedOn(BLUE);    
        LedOn(CYAN);    
        LedOn(GREEN);    
        LedOff(YELLOW);    
        LedOff(ORANGE);    
        LedOff(RED); 
      }
      if(s8Rssi> -68 && s8Rssi<=-60)
      {
        LCDCommand(LCD_CLEAR_CMD);
        LCDMessage(LINE2_START_ADDR+13, au8SeekMessage1);
        LedOn(WHITE);
        LedOn(PURPLE);    
        LedOn(BLUE);    
        LedOn(CYAN);    
        LedOn(GREEN);    
        LedOn(YELLOW);    
        LedOff(ORANGE);    
        LedOff(RED); 
      }
      if(s8Rssi> -60 && s8Rssi<=-52)
      {
        LCDCommand(LCD_CLEAR_CMD);
        LCDMessage(LINE2_START_ADDR+13, au8SeekMessage1);
        LedOn(WHITE);
        LedOn(PURPLE);    
        LedOn(BLUE);    
        LedOn(CYAN);    
        LedOn(GREEN);    
        LedOn(YELLOW);    
        LedOn(ORANGE);    
        LedOff(RED); 
      }
      if(s8Rssi> -52 && s8Rssi<=-48)
      {
        AntCloseChannelNumber(ANT_CHANNEL_USERAPP);
        LCDCommand(LCD_CLEAR_CMD);
        LCDMessage(LINE2_START_ADDR, au8SeekMessage2);
        LedOn(WHITE);
        LedOn(PURPLE);    
        LedOn(BLUE);    
        LedOn(CYAN);    
        LedOn(GREEN);    
        LedOn(YELLOW);    
        LedOn(ORANGE);    
        LedOn(RED);
        s8Rssi = 0;
        AntQueueBroadcastMessage(ANT_CHANNEL_USERAPP, au8FoundMessage);
        u8Role = 2;
        UserApp1_StateMachine = UserApp1SM_Close;
      }
    }
  }
  
}

static void UserApp1SM_Hide(void)
{
  static u8 u8Check = 0;
  u8 au8TestMessage[] = {0,0,0,0,0,0,0,0};
  u8 au8FoundData [] = {2,2,2,2,2,2,2,2};
  u8 au8FoundMessage [] ="   You Found Me   ";
  
  if(AntReadAppMessageBuffer())
  {
    if(G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      for(u8 i=0;i<ANT_DATA_BYTES;i++)
      {
        if(G_au8AntApiCurrentMessageBytes[i] == au8FoundData[i])
        {
          u8Check++;
        }
      }
      if (u8Check == 8)
      {
        AntCloseChannelNumber(ANT_CHANNEL_USERAPP);
        //bShow = TRUE;
        //bSeek = TRUE;
        LCDCommand(LCD_CLEAR_CMD);
        LCDMessage(LINE1_START_ADDR, au8FoundMessage);
        UserApp1_StateMachine = UserApp1SM_Close;
        u8Role = 1;
      }
      u8Check = 0;
    }
    if(G_eAntApiCurrentMessageClass == ANT_TICK)
    {
       AntQueueBroadcastMessage(ANT_CHANNEL_USERAPP, au8TestMessage);
    }
  }
}

static void UserApp1SM_Close(void)
{
  //static bool bChoose = TRUE;
  AntCloseChannelNumber(ANT_CHANNEL_USERAPP);
  LedOff(WHITE);
  LedOff(PURPLE);    
  LedBlink(BLUE,LED_8HZ);    
  LedOff(CYAN);
  LedOff(GREEN);
  LedOff(YELLOW);    
  LedOff(ORANGE);    
  LedOff(RED); 

  UserApp1_StateMachine = UserApp1SM_CloseWait;
}

static void UserApp1SM_CloseWait(void)
{
  static u32 u32TimeCounter = 0;
  u8 au8CloseWaitMessage[]="Please wait 5s";
  
  if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_CLOSED)
  {
    u32TimeCounter++;
  }
  if(u32TimeCounter>=5000)
  {
    LedOff(BLUE);
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, au8CloseWaitMessage);
    u32TimeCounter=0;
    AntUnassignChannelNumber(ANT_CHANNEL_USERAPP);
    UserApp1_StateMachine = UserApp1SM_Choose;
  }
}
  /*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for a message to be queued */
static void UserApp1SM_Idle(void)
{
  /* Look for BUTTON 0 to open channel */
  if(WasButtonPressed(BUTTON0))
  {
    /* Got the button, so complete one-time actions before next state */
    ButtonAcknowledge(BUTTON0);
    
    
    /* Set timer and advance states */
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserApp1SM_Open;
  }
    
} /* end UserApp1SM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/







/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for channel to close */


/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{

} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
