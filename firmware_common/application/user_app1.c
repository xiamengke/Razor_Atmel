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
static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */
//static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */
static u8 u8State = 0;
static bool bRoll = FALSE;
static u16 u16Counter;
static u8 au8LedData[16][10]={0};
static u8 au8RollData[10]={0};
static u8 au8RollData1[16][10]={0};
static u8 au8Prepare[16][10] = {0};
static u8 au8Data[]={0x00,0x10,0x00,0x10,0xFE,0x17,0x08,0x10,
0x08,0xFC,0xC8,0x13,0x48,0x32,0x48,0x3A,
0x48,0x56,0x48,0x52,0xC8,0x93,0x48,0x12,
0x08,0x10,0x08,0x10,0x28,0x10,0x10,0x10};/*"柯",2*/
static u8 au8Data1[]={0x20,0x08,0x20,0x08,0xFC,0x7E,0x20,0x08,
0x70,0x1C,0xA8,0x2A,0x26,0xC8,0x00,0x04,
0xF0,0x07,0x10,0x08,0x20,0x14,0x40,0x22,
0x80,0x01,0x00,0x02,0x00,0x0C,0x00,0x70};/*"梦",0*/
static u8 au8Data2[]={0xFC,0x7F,0x00,0x02,0xF0,0x1F,0x10,0x10,0xF0,0x1F,0x10,0x10,0xF0,0x1F,0x10,0x10,0xF0,0x1F,0x00,0x08,0xF0,0x1F,0x20,0x28,0x40,0x44,0x80,0x03,0x70,0x1C,0x0E,0xE0};/*"夏",0*/





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
  //MBI5026GF
  AT91C_BASE_PIOA->PIO_PER |= (PA_11_BLADE_UPIMO|PA_12_BLADE_UPOMI|PA_14_BLADE_MOSI|PA_15_BLADE_SCK);
  AT91C_BASE_PIOA->PIO_PDR &= ~(PA_11_BLADE_UPIMO|PA_12_BLADE_UPOMI|PA_14_BLADE_MOSI|PA_15_BLADE_SCK);
  AT91C_BASE_PIOA->PIO_OER |= (PA_11_BLADE_UPIMO|PA_12_BLADE_UPOMI|PA_14_BLADE_MOSI|PA_15_BLADE_SCK);
  AT91C_BASE_PIOA->PIO_ODR &= ~(PA_11_BLADE_UPIMO|PA_12_BLADE_UPOMI|PA_14_BLADE_MOSI|PA_15_BLADE_SCK);
  
  //CD4515BM
  AT91C_BASE_PIOA->PIO_PER |= (PA_03_HSMCI_MCCK|PA_04_HSMCI_MCCDA|PA_05_HSMCI_MCDA0|PA_06_HSMCI_MCDA1|PA_07_HSMCI_MCDA2|PA_08_SD_CS_MCDA3);
  AT91C_BASE_PIOA->PIO_PDR &= ~(PA_03_HSMCI_MCCK|PA_04_HSMCI_MCCDA|PA_05_HSMCI_MCDA0|PA_06_HSMCI_MCDA1|PA_07_HSMCI_MCDA2|PA_08_SD_CS_MCDA3);
  AT91C_BASE_PIOA->PIO_OER |= (PA_03_HSMCI_MCCK|PA_04_HSMCI_MCCDA|PA_05_HSMCI_MCDA0|PA_06_HSMCI_MCDA1|PA_07_HSMCI_MCDA2|PA_08_SD_CS_MCDA3);
  AT91C_BASE_PIOA->PIO_ODR &= ~(PA_03_HSMCI_MCCK|PA_04_HSMCI_MCCDA|PA_05_HSMCI_MCDA0|PA_06_HSMCI_MCDA1|PA_07_HSMCI_MCDA2|PA_08_SD_CS_MCDA3);
 
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
  u8 i;
  
  //static u8 u8State = 0;
  
  u16Counter++;
  
  if(u16Counter>=20)
  {
    if(u8State<=127)
    {
      u8State++;
      bRoll=TRUE;
      
     
    }
    /*if(u8State>80)
    {
      u8State = 80;
    }*/
    u16Counter = 0;
    
  }
  

    for(i=0;i<16;i++)
  {
    Rolling(i);
  
    Line_Data(i);
    AT91C_BASE_PIOA->PIO_SODR = PA_11_BLADE_UPIMO;
    Delay(5);
    AT91C_BASE_PIOA->PIO_SODR = PA_12_BLADE_UPOMI;
    Delay(5);
    AT91C_BASE_PIOA->PIO_CODR = PA_12_BLADE_UPOMI;
    Delay(5);
    Hang_Data(i);
    AT91C_BASE_PIOA->PIO_CODR = PA_11_BLADE_UPIMO;
    Delay(5);
  }



    
}/* end UserApp1SM_Idle() */


void Rolling(u8 k)
{
  static bool bPrepare = FALSE;
  static s8 l;
  static u8 u8Shang;
  static u8 u8Yu;
  static u8 u8PrepareData;
  static u8 u8Data;
  static u8 u8Data1;
  u8 i;
  u8 j = 0;
  
  if(bPrepare == FALSE)
  {
    bPrepare = TRUE;
    for(i=0;i<16;i++)
    {
      for(l=1;l>=0;l--)
      {
        au8Prepare[i][l]=au8Data2[j];
        au8Prepare[i][l+2]=au8Data1[j];
        au8Prepare[i][l+4]=au8Data[j];
        j++;
      }
      if(j>=32)
      {
        j=0;
      }
    }
  }
  
  if(bRoll)
  {
    bRoll = FALSE;
    for(i=0;i<16;i++)
    {
      for(j=0;j<10;j++)
      {
        au8RollData1[i][j] = au8LedData[i][j];
      }
    }
  }
  

  if(u8State<80 )
  {
    u8Shang = u8State/8;
    u8Yu = u8State%8;
    for(i=0;i<10;i++)
    {
      au8RollData[i] = au8RollData1[k][i];
    }
    
    //RollData的前几位
    for(i=0;i<u8Shang;i++)
    {  
      u8Data = au8RollData[u8Shang -i];
      u8Data = u8Data<<1;
      u8Data1 = au8RollData[u8Shang-i - 1];
      if((u8Data1 & 0x80) == 0x80)
      {
        au8RollData[u8Shang - i] = u8Data + 0x01;
      }
      else if((u8Data1 & 0x80) == 0)
      {
        au8RollData[u8Shang - i] = u8Data;
      }
    }
    
    //RollData的0位
    u8Data = au8RollData[0];
    u8Data = u8Data<<1;
    u8PrepareData = au8Prepare[k][u8Shang];
    if(u8Yu == 0 && u8Shang>0)
    {
      u8PrepareData = au8Prepare[k][u8Shang-1];
      u8Yu = 8;
    }
    u8PrepareData = u8PrepareData << (u8Yu - 1);
    if((u8PrepareData & 0x80) == 0x80)
    {
      au8RollData[0] = u8Data + 0x01;
    }
    else if ((u8PrepareData & 0x80) == 0)
    {
      au8RollData[0] = u8Data;
    }
    
  }

  
  
  if(u8State>=80)
  {
    for(i=0;i<10;i++)
    {
      au8RollData[i] = au8RollData1[k][i];
    }
    for(i=0;i<10;i++)
    {  
      u8Data = au8RollData[10 -i];
      u8Data = u8Data<<1;
      u8Data1 = au8RollData[10 -i - 1];
      if((u8Data1 & 0x80) == 0x80)
      {
        au8RollData[10 - i] = u8Data + 0x01;
      }
      else if((u8Data1 & 0x80) == 0)
      {
        au8RollData[10- i] = u8Data;
      }
    }
  }
  if(u8State >=128)
  {
    u8State = 0;
  }
}


void Hang_Data(u8 k)
{
  static u8 Counter;
  
  AT91C_BASE_PIOA->PIO_CODR = PA_04_HSMCI_MCCDA;
  AT91C_BASE_PIOA->PIO_SODR = PA_03_HSMCI_MCCK;
  Counter = k;
  if(Counter == 0)
   {
     
      
      AT91C_BASE_PIOA->PIO_CODR = PA_05_HSMCI_MCDA0;
      AT91C_BASE_PIOA->PIO_CODR = PA_06_HSMCI_MCDA1;
      AT91C_BASE_PIOA->PIO_CODR = PA_07_HSMCI_MCDA2;
      AT91C_BASE_PIOA->PIO_CODR = PA_08_SD_CS_MCDA3;
      

   }
   if(Counter == 1)
   {

      
      AT91C_BASE_PIOA->PIO_SODR = PA_05_HSMCI_MCDA0;
      AT91C_BASE_PIOA->PIO_CODR = PA_06_HSMCI_MCDA1;
      AT91C_BASE_PIOA->PIO_CODR = PA_07_HSMCI_MCDA2;
      AT91C_BASE_PIOA->PIO_CODR = PA_08_SD_CS_MCDA3;
      

   }
   if(Counter == 2)
   {

      
      AT91C_BASE_PIOA->PIO_CODR = PA_05_HSMCI_MCDA0;
      AT91C_BASE_PIOA->PIO_SODR = PA_06_HSMCI_MCDA1;
      AT91C_BASE_PIOA->PIO_CODR = PA_07_HSMCI_MCDA2;
      AT91C_BASE_PIOA->PIO_CODR = PA_08_SD_CS_MCDA3;

   }
   if(Counter == 3)
   {
     
      

      
      AT91C_BASE_PIOA->PIO_SODR = PA_05_HSMCI_MCDA0;
      AT91C_BASE_PIOA->PIO_SODR = PA_06_HSMCI_MCDA1;
      AT91C_BASE_PIOA->PIO_CODR = PA_07_HSMCI_MCDA2;
      AT91C_BASE_PIOA->PIO_CODR = PA_08_SD_CS_MCDA3;

   }
   if(Counter == 4)
   {

      AT91C_BASE_PIOA->PIO_CODR = PA_05_HSMCI_MCDA0;
      AT91C_BASE_PIOA->PIO_CODR = PA_06_HSMCI_MCDA1;
      AT91C_BASE_PIOA->PIO_SODR = PA_07_HSMCI_MCDA2;
      AT91C_BASE_PIOA->PIO_CODR = PA_08_SD_CS_MCDA3;
      

   }
   if(Counter == 5)
   {

      
      AT91C_BASE_PIOA->PIO_SODR = PA_05_HSMCI_MCDA0;
      AT91C_BASE_PIOA->PIO_CODR = PA_06_HSMCI_MCDA1;
      AT91C_BASE_PIOA->PIO_SODR = PA_07_HSMCI_MCDA2;
      AT91C_BASE_PIOA->PIO_CODR = PA_08_SD_CS_MCDA3;

   }
   if(Counter == 6)
   {

      
      AT91C_BASE_PIOA->PIO_CODR = PA_05_HSMCI_MCDA0;
      AT91C_BASE_PIOA->PIO_SODR = PA_06_HSMCI_MCDA1;
      AT91C_BASE_PIOA->PIO_SODR = PA_07_HSMCI_MCDA2;
      AT91C_BASE_PIOA->PIO_CODR = PA_08_SD_CS_MCDA3;

   }
   if(Counter == 7)
   {
      

      
      AT91C_BASE_PIOA->PIO_SODR = PA_05_HSMCI_MCDA0;
      AT91C_BASE_PIOA->PIO_SODR = PA_06_HSMCI_MCDA1;
      AT91C_BASE_PIOA->PIO_SODR = PA_07_HSMCI_MCDA2;
      AT91C_BASE_PIOA->PIO_CODR = PA_08_SD_CS_MCDA3;
      

   }
   if(Counter == 8)
   {

      
      AT91C_BASE_PIOA->PIO_CODR = PA_05_HSMCI_MCDA0;
      AT91C_BASE_PIOA->PIO_CODR = PA_06_HSMCI_MCDA1;
      AT91C_BASE_PIOA->PIO_CODR = PA_07_HSMCI_MCDA2;
      AT91C_BASE_PIOA->PIO_SODR = PA_08_SD_CS_MCDA3;

      

   }
   if(Counter == 9)
   {
      

      
      AT91C_BASE_PIOA->PIO_SODR = PA_05_HSMCI_MCDA0;
      AT91C_BASE_PIOA->PIO_CODR = PA_06_HSMCI_MCDA1;
      AT91C_BASE_PIOA->PIO_CODR = PA_07_HSMCI_MCDA2;
      AT91C_BASE_PIOA->PIO_SODR = PA_08_SD_CS_MCDA3;
      

   }
   if(Counter == 10)
   {
      

      
      AT91C_BASE_PIOA->PIO_CODR = PA_05_HSMCI_MCDA0;
      AT91C_BASE_PIOA->PIO_SODR = PA_06_HSMCI_MCDA1;
      AT91C_BASE_PIOA->PIO_CODR = PA_07_HSMCI_MCDA2;
      AT91C_BASE_PIOA->PIO_SODR = PA_08_SD_CS_MCDA3;
      

   }
   if(Counter == 11)
   {
 
      
      AT91C_BASE_PIOA->PIO_SODR = PA_05_HSMCI_MCDA0;
      AT91C_BASE_PIOA->PIO_SODR = PA_06_HSMCI_MCDA1;
      AT91C_BASE_PIOA->PIO_CODR = PA_07_HSMCI_MCDA2;
      AT91C_BASE_PIOA->PIO_SODR = PA_08_SD_CS_MCDA3;
      

   }
 if(Counter == 12)
   {
      
      
 
      
      AT91C_BASE_PIOA->PIO_CODR = PA_05_HSMCI_MCDA0;
      AT91C_BASE_PIOA->PIO_CODR = PA_06_HSMCI_MCDA1;
      AT91C_BASE_PIOA->PIO_SODR = PA_07_HSMCI_MCDA2;
      AT91C_BASE_PIOA->PIO_SODR = PA_08_SD_CS_MCDA3;
      

   }
   if(Counter == 13)
   {

        
      AT91C_BASE_PIOA->PIO_SODR = PA_05_HSMCI_MCDA0;
      AT91C_BASE_PIOA->PIO_CODR = PA_06_HSMCI_MCDA1;
      AT91C_BASE_PIOA->PIO_SODR = PA_07_HSMCI_MCDA2;
      AT91C_BASE_PIOA->PIO_SODR = PA_08_SD_CS_MCDA3;

   }
   if(Counter == 14)
   {
     
      

      
      AT91C_BASE_PIOA->PIO_CODR = PA_05_HSMCI_MCDA0;
      AT91C_BASE_PIOA->PIO_SODR = PA_06_HSMCI_MCDA1;
      AT91C_BASE_PIOA->PIO_SODR = PA_07_HSMCI_MCDA2;
      AT91C_BASE_PIOA->PIO_SODR = PA_08_SD_CS_MCDA3;
      

   }
   if(Counter == 15)
   {

      
      AT91C_BASE_PIOA->PIO_SODR = PA_05_HSMCI_MCDA0;
      AT91C_BASE_PIOA->PIO_SODR = PA_06_HSMCI_MCDA1;
      AT91C_BASE_PIOA->PIO_SODR = PA_07_HSMCI_MCDA2;
      AT91C_BASE_PIOA->PIO_SODR = PA_08_SD_CS_MCDA3;
      

   }
}


void Line_Data(u8 k)
{
  static u8 i;
  u8 j;
  static u8 u8Data;
  
  
  for(i=0;i<10;i++)
  {
    au8LedData[k][i] = au8RollData[i];
    u8Data = au8LedData[k][i];

    for(j=0;j<8;j++)
    {
       AT91C_BASE_PIOA->PIO_CODR = PA_15_BLADE_SCK;
      if((0x01 & u8Data) == 0x00)
      {
        AT91C_BASE_PIOA->PIO_CODR = PA_14_BLADE_MOSI;
      }
      else if((0x01 & u8Data) == 0x01)
      {
        AT91C_BASE_PIOA->PIO_SODR = PA_14_BLADE_MOSI;
      }
      Delay(5);
      AT91C_BASE_PIOA->PIO_SODR = PA_15_BLADE_SCK;
      Delay(5);
      u8Data = u8Data>>1;
    }
  }
  return;
}

void Delay(u8 n)
{
  u8 i;
  for(i=0;i<n;i++)
  {
    
  }
  
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
