/**********************************************************************************************************************
File: user_app1.c                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
 1. Copy both user_app1.c and user_app1.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app1" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

Description:
This is a user_app1.c file template 

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
All Global variable names shall start with "G_UserApp1"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                       /* Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern u32 G_u32AntApiCurrentMessageTimeStamp;                    /* From ant_api.c */
extern AntApplicationMessageType G_eAntApiCurrentMessageClass;    /* From ant_api.c */
extern u8 G_au8AntApiCurrentMessageBytes[ANT_APPLICATION_MESSAGE_BYTES];  /* From ant_api.c */
extern AntExtendedDataType G_sAntApiCurrentMessageExtData;                /* From ant_api.c */

extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_StateMachine;              /* The state machine function pointer */
static u32 UserApp1_u32Timeout;                        /* Timeout counter used across states */

static AntAssignChannelInfoType UserApp1_sChannelInfo; /* ANT setup parameters */

static u8 UserApp1_au8MessageFail[] = "\n\r***ANT channel setup failed***\n\n\r";
static u8 au8TestNumber[10] = {0,0,0,0,0,0,0,0,0,0};
static bool bChoose = TRUE;

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
  u8 au8WelcomeMessage[] = "Welcome";
  u8 ANTHR_NETWORK_KEY[8] = {0xB9, 0xA5, 0x21, 0xFB, 0xBD, 0x72, 0xC3, 0x45};
  
  LCDCommand(LCD_CLEAR_CMD);
  LCDMessage(LINE1_START_ADDR ,au8WelcomeMessage );
 /* Configure ANT for this application */
  UserApp1_sChannelInfo.AntChannel          = ANT_CHANNEL_USERAPP;
  UserApp1_sChannelInfo.AntChannelType      = ANT_CHANNEL_TYPE_USERAPP;
  UserApp1_sChannelInfo.AntChannelPeriodLo  = ANT_CHANNEL_PERIOD_LO_USERAPP;
  UserApp1_sChannelInfo.AntChannelPeriodHi  = ANT_CHANNEL_PERIOD_HI_USERAPP;
 
  UserApp1_sChannelInfo.AntDeviceIdLo       = ANT_DEVICEID_LO_USERAPP;
  UserApp1_sChannelInfo.AntDeviceIdHi       = ANT_DEVICEID_HI_USERAPP;
  UserApp1_sChannelInfo.AntDeviceType       = ANT_DEVICE_TYPE_USERAPP;
  UserApp1_sChannelInfo.AntTransmissionType = ANT_TRANSMISSION_TYPE_USERAPP;
  UserApp1_sChannelInfo.AntFrequency        = ANT_FREQUENCY_USERAPP;
  UserApp1_sChannelInfo.AntTxPower          = ANT_TX_POWER_USERAPP;

  UserApp1_sChannelInfo.AntNetwork = ANT_NETWORK_DEFAULT;
  for(u8 i = 0; i < ANT_NETWORK_NUMBER_BYTES; i++)
  {
    UserApp1_sChannelInfo.AntNetworkKey[i] = ANTHR_NETWORK_KEY[i];
  }
  
  /* Attempt to queue the ANT channel setup */
  if( AntAssignChannel(&UserApp1_sChannelInfo) )
  {
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserApp1SM_AntChannelAssign;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    DebugPrintf(UserApp1_au8MessageFail);
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
/* Wait for ANT channel assignment */
static void UserApp1SM_AntChannelAssign()
{
  if( AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_CONFIGURED)
  {
    /* Channel assignment is successful, so open channel and
    proceed to Idle state */
    AntOpenChannelNumber(ANT_CHANNEL_USERAPP);
    UserApp1_StateMachine = UserApp1SM_RealTimeShow;
  }
  
  /* Watch for time out */
  if(IsTimeUp(&UserApp1_u32Timeout, 3000))
  {
    DebugPrintf(UserApp1_au8MessageFail);
    UserApp1_StateMachine = UserApp1SM_Error;    
  }
     
} /* end UserApp1SM_AntChannelAssign */

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */


static void UserApp1SM_RealTimeShow(void)
{
  u8 u8Hint[] = "HR:";
  u8 u8Hint1[] = "B0:Function";
  static u8 u8HR = 55;
  static bool bOpen = FALSE;
  static u32 u32TimeCounter = 0;
  static u8 u8HRNumber = 0;
  
  u32TimeCounter++;
  
  if(u32TimeCounter == 2000)
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR ,u8Hint);
    LCDMessage(LINE2_START_ADDR ,u8Hint1);
  }
  
  if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_OPEN)
  {
    bOpen = TRUE;
    LedOn(GREEN);
  }
  
  if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) != ANT_OPEN)
  {
    LedOff(GREEN);
  }
  
  if(bOpen)
  {
    if( AntReadAppMessageBuffer())
    {
      if(G_eAntApiCurrentMessageClass == ANT_DATA )
      {
        static u8 i=0;
        if(i<=9)
        {
          if(u8HRNumber != G_au8AntApiCurrentMessageBytes[7])
          {
            u8HRNumber = G_au8AntApiCurrentMessageBytes[7];
            au8TestNumber[i] = u8HRNumber;
            i++;
          }
        }
      
        if(i>9)
        {
          i=0;
        }
        u8HR = HexToDec(G_au8AntApiCurrentMessageBytes[7]);
        ShowRate();
        LedOn(BLUE);
      }
    }
  }
  if(u8HR<=50 )
  {
    LedOn(RED);
  }
  
  if (WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    UserApp1_StateMachine = UserApp1SM_Function;
  }
}

static void UserApp1SM_Function(void)
{
  u8 u8Function1[] = "B1:Max&Min";
  u8 u8Function2[] = "B2:Average";
  u8 u8Function3[] = "B3:HR";
  ////
  
  if(bChoose)
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR ,u8Function1);
    LCDMessage(LINE2_START_ADDR ,u8Function2);
    LCDMessage(LINE2_START_ADDR+11 ,u8Function3);
    bChoose = FALSE;
  }
  if (WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);
    UserApp1_StateMachine = UserApp1SM_MaxMin;
  }
  if (WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);
    UserApp1_StateMachine = UserApp1SM_Average;
  }
  if (WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON3);
    UserApp1_StateMachine = UserApp1SM_RealTimeShow;
  }
}

static void UserApp1SM_MaxMin(void)
{
  static u8 au8Max[3]={0,0,0};
  static u8 au8Min[3]={0,0,0};
  static u8 u8Max = 0;
  static u8 u8Min = 1000;
  u8 au8UserAppMax[] = "Max:";
  u8 au8UserAppMin[] = "Min:";
  u8 au8Function[] = "B0: Function";
  static u8 u8Compare = 0;/////////
  
  static u8 j = 0;
  if(u8Compare == 0)
  {
    
    if(au8TestNumber[j] != 0)
    {
      if(u8Max<=au8TestNumber[j])
      {
        u8Max = HexToDec(au8TestNumber[j]);
        au8Max[0] = (u8Max/100) + '0';
        au8Max[1] = ( (u8Max%100)/10 ) + '0';
        au8Max[2] = ( (u8Max%100)%10 ) + '0'; 
      }
      if(u8Min>=au8TestNumber[j])
      {
        u8Min = HexToDec(au8TestNumber[j]);
        au8Min[0] = (u8Min/100) + '0';
        au8Min[1] = ( (u8Min%100)/10 ) + '0';
        au8Min[2] = ( (u8Min%100)%10 ) + '0'; 
      }
      j++;
    }
    if(au8TestNumber[j] == 0)
    {
      u8Compare = 1;
    } 
  }
  if(u8Compare == 1)
  {
    u8Compare = 2;
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR ,au8UserAppMax);
    LCDMessage(LINE1_START_ADDR + 5 ,au8Max);
    LCDMessage(LINE2_START_ADDR ,au8UserAppMin);
    LCDMessage(LINE2_START_ADDR + 5 ,au8Min);
    LCDMessage(LINE2_START_ADDR + 9 ,au8Function);
  }
  
  if (WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    bChoose = TRUE;
    UserApp1_StateMachine = UserApp1SM_Function;
  }
}
static void UserApp1SM_Average(void)
{
  static u32 u32Sum;
  static u8 u8Number;
  static u8 u8Average;
  static u8 au8AverageNumber[3]={0,0,0};
  static u8 au8Average[] = "Average:";
  static u8 u8Show = 0;
  
  static u8 i;
  for(;i<=9 ; i++)
  {
    u32Sum = u32Sum+au8TestNumber[i];
  }
  static u8 j =0;
  if(au8TestNumber[j] != 0)
  {
    u8Number++;
    j++;
  }
  if(au8TestNumber[j] == 0)
  {
    u8Average = u32Sum / u8Number;
    au8AverageNumber[2] = u8Average;
    u8Average = HexToDec(au8AverageNumber[2]);
    au8AverageNumber[0] = (u8Average/100) + '0';
    au8AverageNumber[1] = ( (u8Average%100)/10 ) + '0';
    au8AverageNumber[2] = ( (u8Average%100)%10 ) + '0'; 
  }
  if(u8Show == 0)
  {
    LCDCommand(LCD_CLEAR_CMD);
    
    u8Show =1;
  }LCDMessage(LINE1_START_ADDR ,au8Average);
    LCDMessage(LINE1_START_ADDR+9 ,au8AverageNumber);
}

static void ShowRate(void)
{
  LCDCommand(LCD_CLEAR_CMD);
  u8 au8ShowMessage[] = "HR:";
   u8 u8Hint1[] = "B0:Function";
  u8 u8RateDec = 0;
  u8 au8Show[3] = {0,0,0};
  
  u8RateDec = HexToDec(G_au8AntApiCurrentMessageBytes[7]);
  au8Show[0] = (u8RateDec/100) + '0';
  au8Show[1] = ( (u8RateDec%100)/10 ) + '0';
  au8Show[2] = ( (u8RateDec%100)%10 ) + '0'; 
  
  if (au8Show[0] == '0')
  {
    au8Show[0] = ' ';
  }
  
  LCDMessage(LINE1_START_ADDR, au8ShowMessage);
  LCDMessage(LINE1_START_ADDR + 5, au8Show);
  LCDMessage(LINE2_START_ADDR ,u8Hint1);
  return;
}

u8 HexToDec(u8 u8Char_)
{
  u8 au8Change[2] = {0,0};
  u8 u8ReturnValue = 0;
  
  au8Change[0] = (u8Char_ /16) * 16;
  au8Change[1] = u8Char_ %16;
  u8ReturnValue = au8Change[0] + au8Change[1];
  return(u8ReturnValue);
}
/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error (for now, do nothing) */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/