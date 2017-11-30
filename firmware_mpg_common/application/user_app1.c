/**********************************************************************************************************************
File: user_app1.c                                                                

Description:
Provides a Tera-Term driven system to display, read and write an LED command list.

Test1.
Test2 from Engenuics.
Test3.

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:
None.

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
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */

extern u8 G_au8DebugScanfBuffer[DEBUG_SCANF_BUFFER_SIZE]; /* From debug.c */
extern u8 G_u8DebugScanfCharCount;                        /* From debug.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */
//static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */
static u8 au8UserMenu1[]="************************************\n\rPress 1 to program LED command sequence\n\rPress 2 to show current USER program\n\r************************************\n\r";
static u8 au8UserMessage1[]="\n\rEnter commands as LED-ONTIME-OFFTIME and press Enter\n\rTime is in milliseconds, max 100 commands";
static u8 au8UserMessage2[]="\n\rLED colors: R(r),O(o),Y(y),G(g),C(c),B(b),P(p),W(w)\n\rExample: R-100-200 (RED on at 100ms and off at 200ms)\n\rPress Enter on blank line to end\n\r";
static u8 au8UserMessage3[]="\n\rLED   ON TIME   OFF TIME\n\r-----------------------------------\n\r";
static u8 au8Error[]="\n\rThe command is wrong\n\r";
static u8 au8Error1[]="\n\rThe LedName is wrong\n\r";
static u8 au8Error2[]="\n\rThe charcter after LedName is not '-'\n\r";
static u8 au8Error3[]="\n\rStart time is not number\n\r";
static u8 au8Error4[]="The charcter after Start time is not '-'\n\r";
static u8 au8Error5[]="\n\rEnd time is not number\n\r";
static u8 au8Error6[]="\n\rStart time is larger than End time\n\r";

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
  u8 au8UserApp1Start1[] = "LED program task started\n\r";
  
  /* Turn off the Debug task command processor and announce the task is ready */
  DebugSetPassthrough();
  DebugPrintf(au8UserApp1Start1);
  DebugLineFeed();
  DebugPrintf(au8UserMenu1);
  
    /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_StateMachine = UserApp1SM_FailedInit;
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
/* Wait for input */
static void UserApp1SM_Idle(void)
{
  static u8 u8Pattern = 0;
  static u32 u32DebugNumber=0;
  static u8 au8Debug[20];
  static u8 u8Right = 0;
  static u8 u8Error = 0;
  static bool bJudge = FALSE;
  static u8 au8LedNames[]={WHITE,WHITE, PURPLE, BLUE, CYAN, GREEN, YELLOW, ORANGE, RED};
  static u8 u8Colour1 = 0;
  static u8 u8Colour2 = 0;
  static u8 u8SignNumber = 0;
  static u32 u32StartTime = 0;
  static u32 u32EndTime = 0;
  static bool bAgain = FALSE;
  static LedCommandType X;
  static u8 u8List = 0;
  static u8 u8Count = 0;
  
  if(u8Pattern==0)
  {
    if(G_au8DebugScanfBuffer[G_u8DebugScanfCharCount-1] == 0x31)
    {
      u8Pattern=DebugScanf(G_au8DebugScanfBuffer);
      DebugPrintf(au8UserMessage1);
      DebugPrintf(au8UserMessage2);
      DebugLineFeed();
      LedDisplayStartList();
    }
    if(G_au8DebugScanfBuffer[G_u8DebugScanfCharCount-1] == 0x32)
    {
      u8Count=0;
      u8Pattern=DebugScanf(G_au8DebugScanfBuffer)+1;
      DebugPrintf(au8UserMessage3);
      DebugLineFeed();
    }
    else if(G_au8DebugScanfBuffer[G_u8DebugScanfCharCount-1]!=0x00)
    {
      u8Pattern=3;
    } 
  }
  //Main menu
  
  if(u8Pattern==1)
  {
    if(G_au8DebugScanfBuffer[G_u8DebugScanfCharCount-1] == 0x0D)
    {
      if(G_u8DebugScanfCharCount != 1)
      {
        bJudge=TRUE;
        for(u16 i=0;i<=G_u8DebugScanfCharCount-1;i++)
        {
          au8Debug[i]=G_au8DebugScanfBuffer[i];
        }
        u32DebugNumber=DebugScanf(G_au8DebugScanfBuffer);
      }
      if(G_u8DebugScanfCharCount == 1)
      {
        bJudge=FALSE;
        u8Pattern=0;
        u32DebugNumber=DebugScanf(G_au8DebugScanfBuffer);
      }
    }
    
    if(bJudge==TRUE)
    {
      if(u8Error==0)
      {
        if( u8Right ==0)
        {
          static u8 au8Colour[2][8]={{'W','P','B','C','G','Y','O','R'},
          {'w','p','b','c','g','y','o','r'}};
          if(u8Colour1<2)
          {
            if(u8Colour1==0)
            {
              if(au8Debug[0]==au8Colour[0][u8Colour2])
              {
                
                u8Right++;
              }
              u8Colour2++;
            }
            if(u8Colour1==1)
            {
              if(au8Debug[0]==au8Colour[1][u8Colour2])
              {
                
                u8Right++;
              }
              u8Colour2++;
            }
            if(u8Colour2>8)
            {
              u8Colour1++;
              u8Colour2=0;
            }
          }
          else if(u8Right==0)
          {
            u8Error=1;
          }
        }
        //Judge Ledname
        
        if(u8Right==1)
        {
          if(au8Debug[1]==0x2D)
          {
            u8Right++;
          }
          else
          {
            u8Error=2;
          }
        }
        //Judge the first '-'
        
        if(u8Right==2)
        {
          for(u16 i=2;i<=8;i++ )
          {
            if(au8Debug[i]==0x2D)
            {
              u8SignNumber = i;
              u8Right++;
            }
            if(i==8 && u8SignNumber==0)
            {
              u8Error=4;
            }
          }
        }
        //Judge the second '-'
        
        if(u8Right==3)
        {
          for(u16 i=2;i<=u32DebugNumber-1;i++ )
          {
            static u32 u32Start1 = 0;
            static u32 u32End1 = 0;
            if(i<u8SignNumber)
            {
              if(au8Debug[i]>='0' && au8Debug[i]<='9')
              {
                u32Start1=au8Debug[i]-'0';
                for(u16 m=0;m<(u8SignNumber-i-1);m++)
                {
                  u32Start1=u32Start1*10;
                }
                u32StartTime=u32StartTime+u32Start1;
              }
              else
              {
                u8Error=3;
              }
            }
            //Start Time
            if(i>u8SignNumber && i<u32DebugNumber-1)
            {
              if(au8Debug[i]>='0' && au8Debug[i]<='9')
              {
                u32End1=au8Debug[i]-'0';
                for(u16 m=0;m<(u32DebugNumber-i-2);m++)
                {
                  u32End1=u32End1*10;
                }
                u32EndTime=u32EndTime+u32End1;
              }
              else 
              {
                u8Error=5;
              }
            }
            //End Time
            if(au8Debug[i]==0x0D)
            {
              u8Right++;
            }
          } 
        }
        //Judge Start Time and End Time
        
        if(u8Right==4 && u8Error==0)
        {
          if(u32StartTime>=u32EndTime)
          {
            u8Error=6;
          }
          else
          {
            u8Right++;
          }
        }
        //Judge Start Time smaller than End Time
        
        if(u8Right==5)
        {
          if(bAgain==FALSE)
          {
            X.eLED = au8LedNames[u8Colour2];
            X.u32Time = u32StartTime;
            X.bOn = TRUE;
            X.eCurrentRate = LED_PWM_100;
            LedDisplayAddCommand(USER_LIST , &X);
            
            X.eLED = au8LedNames[u8Colour2];
            X.u32Time = u32EndTime;
            X.bOn = FALSE;
            X.eCurrentRate = LED_PWM_0;
            LedDisplayAddCommand(USER_LIST , &X);
            bAgain=TRUE;
            u8List++;
          }
          if(bAgain)
          {
            for(u16 i=0;i<=20;i++)
            {
              au8Debug[i]=0x00;
            }
            u8Right=0;
            u8Error=0;
            u8Colour1 = 0;
            u8Colour2 = 0;
            u8SignNumber = 0;
            u32StartTime = 0;
            u32EndTime = 0;
            bJudge = FALSE;
            bAgain = FALSE;
            DebugLineFeed();
          }
        }
      }
      
      if(u8Error!=0)
      {
        for(u16 i=0;i<=20;i++)
        {
          au8Debug[i]=0x00;
        }
        u8Right=0;
        
        u8Colour1 = 0;
        u8Colour2 = 0;
        u8SignNumber = 0;
        u32StartTime = 0;
        u32EndTime = 0;
        bJudge = FALSE;
        bAgain = FALSE;
        switch(u8Error)
        {
        case 1 :
          u8Error=0;
          DebugPrintf(au8Error1);
          break;
        case 2 :
          u8Error=0;
          DebugPrintf(au8Error2);
          break;
        case 3 :
          u8Error=0;
          DebugPrintf(au8Error3);
          break;
        case 4:
          u8Error=0;
          DebugPrintf(au8Error4);
          break;
        case 5 :
          u8Error=0;
          DebugPrintf(au8Error5);
          break;
        case 6 :
          u8Error=0;
          DebugPrintf(au8Error6);
          break;
        }
      }
      //The form of error
    }
  }
  
  if(u8Pattern==2)
  {
    u8Pattern=0;
    for(u8Count=0;u8Count<=u8List;u8Count++)
    {
      LedDisplayPrintListLine(u8Count);
    }
    DebugPrintf(au8UserMenu1);
  }
  //Show
  
  if(u8Pattern==3)
  {
    u8Pattern=0;
    DebugPrintf(au8Error); 
    DebugPrintf(au8UserMenu1);
    u8Colour2=DebugScanf(G_au8DebugScanfBuffer);
  }

  
} /* end UserApp1SM_Idle() */
                      
            
#if 0
/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */
#endif


/*-------------------------------------------------------------------------------------------------------------------*/
/* State to sit in if init failed */
static void UserApp1SM_FailedInit(void)          
{
    
} /* end UserApp1SM_FailedInit() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
