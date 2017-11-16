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
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */
extern u8 G_au8DebugScanfBuffer[];                      /* From debug.c */
extern u8 G_u8DebugScanfCharCount;                      /* From debug.c */

/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */
//static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */
static u8 UserApp_au8Menu1[] = "B0:Name  B1:User";
static u8 UserApp_au8Menu2[] = "B3:Menu";
static u8 UserApp_CursorPosition;
static u8 UserApp_CursorPosition1=1;
static u8 UserApp_CursorPosition2=2;
static u16 au16Tone[]={0,523,578,628,698,784,880,988 };

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
  LCDCommand(LCD_CLEAR_CMD);
  LCDMessage(LINE1_START_ADDR, UserApp_au8Menu1);
  LCDMessage(LINE2_START_ADDR, UserApp_au8Menu2);
  
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
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
/* Wait for ??? */
static void UserApp1SM_Idle(void)
{
  static u32 u32Counter=0;
  static u8  u8Sign = 0;
  static u8  u8Sign1 = 0;
  static u8 u8Tone = 0;
  
  static u8 u8CharCount = 0;
  static u32 u32Number1 = 0;
  static u32 u32Number2 = 0;
  static bool bLine1 = TRUE;
  static u8 u8Print = 0;
  static u8 u8Menu = 0;
  static u8 au8Message[]="Please input:";
  
  
  u32Counter++; 
  if(u8Menu==0)
  {
    if( WasButtonPressed(BUTTON0) )
    {
      ButtonAcknowledge(BUTTON0);
      u8Menu=1;
    }
    if( WasButtonPressed(BUTTON1) )
    {
      ButtonAcknowledge(BUTTON1);
      u8Menu=2;
    }
  }
  if( WasButtonPressed(BUTTON3) )
  {
    ButtonAcknowledge(BUTTON3);
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, UserApp_au8Menu1);
    LCDMessage(LINE2_START_ADDR, UserApp_au8Menu2);
    PWMAudioOff(BUZZER1);
    LedOff(RED);
    LedOff(BLUE);
    LedOff(GREEN);
    u8Print = 0;
    u8Menu=0;
    u8Sign = 0;
    u8Sign1 = 0;
    u8Tone = 0; 
    u8CharCount = 0;
    u32Number1 = 0;
    u32Number2 = 0;
  }
  
  if(u8Menu==1)
  {
    if( WasButtonPressed(BUTTON1) )
    {
      ButtonAcknowledge(BUTTON1);
      PWMAudioOff(BUZZER1);
      u8Sign1=1;
      u8Sign=0;
      
    }
    if( WasButtonPressed(BUTTON0) )
    {
      ButtonAcknowledge(BUTTON0);
      PWMAudioOff(BUZZER1);
      u8Sign1=2;
      u8Sign=0;
      
    }
    
    if(u32Counter >= 500 && u8Sign == 0)
    {
      u32Counter=0;
      PWMAudioSetFrequency(BUZZER1, au16Tone[u8Tone]);
      PWMAudioOn(BUZZER1);
      LCDCommand(LCD_CLEAR_CMD);
      LCDMessage(UserApp_CursorPosition, "X");
      LCDMessage(UserApp_CursorPosition1, "M");
      LCDMessage(UserApp_CursorPosition2, "K");
      
      
      if(u8Sign1==1)
      {
        LedOff(RED);
        LedOff(BLUE);
        LedOn(GREEN);
        u8Tone++;
        if(UserApp_CursorPosition2 == LINE2_END_ADDR)
        {
          u8Sign=1;
        }
        if(UserApp_CursorPosition == LINE1_END_ADDR)
        {
          UserApp_CursorPosition = LINE2_START_ADDR;
        }
        else
        {
          UserApp_CursorPosition++;
        }
        
        if(UserApp_CursorPosition1 == LINE1_END_ADDR)
        {
          UserApp_CursorPosition1 = LINE2_START_ADDR;
        }
        else
        {
          UserApp_CursorPosition1++;
        }
        
        if(UserApp_CursorPosition2 == LINE1_END_ADDR)
        {
          UserApp_CursorPosition2 = LINE2_START_ADDR;
        }
        else
        {
          UserApp_CursorPosition2++;
        }
      }
      
      if(u8Sign1==2)
      {
        LedOff(RED);
        LedOn(BLUE);
        LedOff(GREEN);
        u8Tone--;
        if(UserApp_CursorPosition == LINE1_START_ADDR)
        {
          u8Sign=1;
        }
        if(UserApp_CursorPosition2 == LINE2_START_ADDR)
        {
          UserApp_CursorPosition2 = LINE1_END_ADDR;
        }
        else
        {
          UserApp_CursorPosition2--;
        }
        
        if(UserApp_CursorPosition1 == LINE2_START_ADDR)
        {
          UserApp_CursorPosition1 = LINE1_END_ADDR;
        }
        else
        {
          UserApp_CursorPosition1--;
        }
        
        if(UserApp_CursorPosition == LINE2_START_ADDR)
        {
          UserApp_CursorPosition = LINE1_END_ADDR;
        }
        else
        {
          UserApp_CursorPosition--;
        }
        
      }
    }
    
    if(u8Tone>8 && u8Sign1==1)
    {
      u8Tone=1;
    }
    if(u8Tone==0 && u8Sign1==2)
    {
      u8Tone=8;
    }
    
    if(u8Sign==1)
    {
      LedOn(RED);
      LedOff(BLUE);
      LedOff(GREEN);
      PWMAudioSetFrequency(BUZZER1, 500);
      PWMAudioOn(BUZZER1);
    }
  }
  
  if(u8Menu==2)
  {
    if(u8Print==0)
    {
      LCDCommand(LCD_CLEAR_CMD);
      DebugPrintf(au8Message);
      u8Print=1;
    }
    
    if(G_u8DebugScanfCharCount==1)
    {
      if(G_au8DebugScanfBuffer[0]==0x0D)
      {
        if(bLine1==TRUE)
        {
          u32Number1=20;
        }
        if(bLine1==FALSE)
        {
          u32Number2=20;
        }
        u8CharCount=DebugScanf(G_au8DebugScanfBuffer);
        u8Print=1;
      }
      
      if(bLine1==TRUE)
      {
        LCDMessage(LINE1_START_ADDR+u32Number1,G_au8DebugScanfBuffer);
        u8CharCount=DebugScanf(G_au8DebugScanfBuffer);
        u32Number1++;
      }
      if(bLine1==FALSE)
      {
        LCDMessage(LINE2_START_ADDR+u32Number2,G_au8DebugScanfBuffer);
        u8CharCount=DebugScanf(G_au8DebugScanfBuffer);
        u32Number2++;
      }
      
      if(u32Number1>20)
      {
        u32Number1=0;
        u32Number2=0;
        u8Print=1;
        bLine1=FALSE; 
        
      }
      if(u32Number2>20)
      {
        u32Number1=0;
        u32Number2=0;
        u8Print=1;
        bLine1=TRUE;
      }
    }
  }

} /* end UserApp1SM_Idle() */
    

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
