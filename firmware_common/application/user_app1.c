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


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_StateMachine; 
static u8 u8States = 2;
static u8 u8Array = 5;
static u8 au8Message1[]="MIC";
static u8 au8Message2[]="Phone";
static u8 au8Message3[]="MUTE";
static u8 au8Array[1]="";
     

/* The state machine function pointer */
//static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */


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
   AT91C_BASE_PIOA->PIO_CODR = PA_14_BLADE_MOSI;
   AT91C_BASE_PIOA->PIO_CODR = PA_11_BLADE_UPIMO;
   AT91C_BASE_PIOB->PIO_CODR = PB_04_BLADE_AN1;

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
     
   if(WasButtonPressed(BUTTON3))
   {
     ButtonAcknowledge(BUTTON3);
     u32 g;
     for(g=0;g<=2000;g++)
     {
        LedOn(RED);
     }
        LedOff(RED);
     UserApp1_StateMachine = UserApp1SM_Pattern;
     
  }
    
    if(WasButtonPressed(BUTTON0))
   {
      ButtonAcknowledge(BUTTON0);
       u32 g;
       for(g=0;g<=2000;g++)
         {
           LedOn(RED);
         }
           LedOff(RED);
       UserApp1_StateMachine = UserApp1SM_Increase;
      
  } 
    
     if(WasButtonPressed(BUTTON1))
   {
     ButtonAcknowledge(BUTTON1); 
     u32 g;
     for(g=0;g<=2000;g++)
   {
      LedOn(RED);
   }
      LedOff(RED);
      UserApp1_StateMachine = UserApp1SM_Reduction;
   }
     
    if(WasButtonPressed(BUTTON2))
   {
      ButtonAcknowledge(BUTTON2);
      LedOn(WHITE);
      
      UserApp1_StateMachine = UserApp1SM_ADC;
      
   }

} /* end UserApp1SM_Idle() */
 static void UserApp1SM_Pattern(void)
{
  
  if(u8States < 4  )
     {
        u8States++;
     }
     if(u8States == 4)
     {
       u8States = 1;
     }
    
     
       if(u8States == 1)
       {
         AT91C_BASE_PIOA->PIO_SODR = PA_14_BLADE_MOSI;
         AT91C_BASE_PIOA->PIO_CODR = PA_11_BLADE_UPIMO;
         AT91C_BASE_PIOB->PIO_CODR = PB_04_BLADE_AN1;
         //MIC PATTERN
         LedOn(BLUE);
         LedOff(GREEN);
         LedOff(PURPLE);
         //THE BLUE LIGHES
         LCDCommand(LCD_CLEAR_CMD);
         LCDMessage(LINE1_START_ADDR, au8Message1);
         //LCD SHOW
      }
       if(u8States == 2)
       {
          AT91C_BASE_PIOA->PIO_CODR = PA_14_BLADE_MOSI;
          AT91C_BASE_PIOA->PIO_CODR = PA_11_BLADE_UPIMO;
          AT91C_BASE_PIOB->PIO_CODR = PB_04_BLADE_AN1;
          //Phone PATTERN
          LedOff(BLUE);
          LedOn(GREEN);
          LedOff(PURPLE);
          //THE GREEN LIGHTS
          LCDCommand(LCD_CLEAR_CMD);
          LCDMessage(LINE1_START_ADDR, au8Message2);
          //LCD SHOW
          
       }
       if(u8States == 3)
       {
          AT91C_BASE_PIOA->PIO_CODR = PA_14_BLADE_MOSI;
          AT91C_BASE_PIOA->PIO_CODR = PA_11_BLADE_UPIMO;
          AT91C_BASE_PIOB->PIO_SODR = PB_04_BLADE_AN1;
          //MUTE PATTERN
          LedOff(BLUE);
          LedOff(GREEN);
          LedOn(PURPLE);
          //THE PURPLE LIGHTS
          LCDCommand(LCD_CLEAR_CMD);
          LCDMessage(LINE1_START_ADDR, au8Message3);
          //LCD SHOW
       }
         UserApp1_StateMachine = UserApp1SM_Idle;
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Increase(void)
{
      au8Array[0] = u8Array + 48;
      u8 i;
      u16 j;
      AT91C_BASE_PIOA->PIO_SODR = PA_13_BLADE_MISO;
      AT91C_BASE_PIOA->PIO_CODR = PA_16_BLADE_CS;
      for(i=0;i<=20;i++)
      {
        AT91C_BASE_PIOA->PIO_SODR = PA_12_BLADE_UPOMI;
        for(j=0;j<=100;j++)
        {
        }
        AT91C_BASE_PIOA->PIO_CODR = PA_12_BLADE_UPOMI;
      }
      //VOLUME INCREASE
       if(u8Array < 9)
       {
        u8Array++;
       }
       if(u8Array > 9)
       {
        u8Array = 9;
       }
        LCDCommand(LCD_CLEAR_CMD);
        LCDMessage(LINE2_START_ADDR, au8Array);
        LCDMessage(LINE1_START_ADDR, au8Message2);
        //LCD SHOW
        UserApp1_StateMachine = UserApp1SM_Idle;
}

static void UserApp1SM_Reduction(void)
{    
     au8Array[0] = u8Array + 48;
     u8 i;
     u16 j;
     AT91C_BASE_PIOA->PIO_CODR = PA_13_BLADE_MISO;
     AT91C_BASE_PIOA->PIO_CODR = PA_16_BLADE_CS;
     for(i = 0;i<=20;i++)
    {
       AT91C_BASE_PIOA->PIO_SODR = PA_12_BLADE_UPOMI;        
       for(j=0;j<=100;j++)
       {
       }
       AT91C_BASE_PIOA->PIO_CODR = PA_12_BLADE_UPOMI;
    }
      //VOLUME REDUCTION
     if(u8Array > 1)
      {
       u8Array--;
      }
      if(u8Array < 1)
      {
        u8Array = 1;
      }
      LCDCommand(LCD_CLEAR_CMD);
      LCDMessage(LINE2_START_ADDR, au8Array);
      LCDMessage(LINE1_START_ADDR, au8Message2);
      //LCD SHOW
      UserApp1_StateMachine = UserApp1SM_Idle;
}

static void UserApp1SM_ADC(void)
{
   static u32 u32Counter;
   u32 g;
   AT91C_BASE_PIOA->PIO_SODR = PA_11_BLADE_UPIMO;
   AT91C_BASE_PIOA->PIO_SODR = PA_15_BLADE_SCK;
   Adc12StartConversion(ADC12_CH2);
   //OPEN CHANNEL
   if(Adc12StartConversion(ADC12_CH2) ==TRUE)
  {
    u32Counter = AT91C_BASE_ADC12B->ADC12B_CDR[ADC12_CH2];
    u32Counter = u32Counter/41;
    u8Array = u32Counter/10;  
   }
   //AD TRANSITION
   for(g=0;g<=2000;g++)
   {
     
   }
     
   LedOff(WHITE);
   AT91C_BASE_PIOA->PIO_CODR = PA_15_BLADE_SCK;
   UserApp1_StateMachine = UserApp1SM_Idle;
}
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
