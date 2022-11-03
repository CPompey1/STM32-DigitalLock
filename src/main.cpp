/*********************************************
Author: Cristian Pompey
Date: Fall 2022
Course: CSE 321
Project: CSE321-Project2- DigitalLock
Last Edit: 11/3/22

Summary of the file:
    Pin connections
        Pins  PA8, PA9, PA10, PA11 will be used as interrupts driven input pins
        Pins  PC8, PC9, PC10,PC11 be used as outputs for columns 0,1,2, and 3 of matrix keypad
        Pins PA0,PA1 PA2,(CH11) PA3 will be used for LED's
        PB8 and PB9 will be used as output i2c pins

    void main(void)
        Main runloop for program

    void col0_handler(void)
        ISR for col 0 detecting a voltage

    void col1_handler(void)
        ISR for col 1 detecting a voltage

    void col2_handler(void)
        ISR for col 2 detecting a voltage

    void col3_handler(void)
        ISR for col 3 detecting a voltage

    void setAsLocked(CSE321_LCD *lcd0)
        Sets system to be locked

    void setAsUnlocked(CSE321_LCD *lcd0)
        Sets system to be unlocked
    bool parseInput(CSE321_LCD *lcd0)
        Parses input currently stored in input buffer

    int32_t bit2Dec(uint8_t bit)
        converts a bit position into the decimal representation

    void cycleRows(void)
        Rapidly applies a voltatge to each row

    void resetPwd(CSE321_LCD *lcd0)
        Routine for resetting passcode

    void gpioOn(char port, int8_t pin)
        sets the given pin for the given port as a logical 1

    void gpioOff(char port, int8_t pin)
        sets the given pin for the given port as a logical 0
    
References: 
*********************************************/
#include "PinNames.h"
#include "mbed.h"
#include <cstdint>
#include <math.h>
#include "lcd1602.h"
#include <string>


#define WORD 32
#define LCDCOL 16
#define LCDROW 2
#define PASSCODE_SIZE 4

//Function definitions
bool parseInput(CSE321_LCD *lcd0);
void resetPwd(CSE321_LCD *lcd0);
int32_t bit2Dec(uint8_t bit);
void col0_handler(void);
void col1_handler(void);
void col2_handler(void);
void col3_handler(void);
void setAsLocked(CSE321_LCD *lcd0);
void setAsUnlocked(CSE321_LCD *lcd0);
void cycleRows(void);
void gpioOff(char port, int8_t pin);
void gpioOn(char port, int8_t pin);

//Global variables
char inputBuf[PASSCODE_SIZE +1] ={"\0\0\0\0"};
char passCode[PASSCODE_SIZE+1] = {"6642"};
char REST_PWD_CMD[5] = {"####"};
int8_t bufCounter = 0;
bool bufHandled = true;
char test[4];

//Enable input pins as interrupts
InterruptIn col0(PA_8,PullDown);
InterruptIn col1(PA_9,PullDown);
InterruptIn col2(PA_10,PullDown);
InterruptIn col3(PA_11,PullDown);


int main()
{
       
    
    //Enable clock for i2c1
    RCC->APB1ENR1 |= bit2Dec(21);

    // Enable output pin clock
    RCC->AHB2ENR |= bit2Dec(2);

    
    //Enable clock for interrupt
    RCC->AHB2ENR |= bit2Dec(0);

    //Enable MODER pins for output LEDs
    //PA0
    GPIOA->MODER &= ~(bit2Dec(1));
    GPIOA->MODER |= bit2Dec(0);

    //PA1
    GPIOA->MODER &= ~(bit2Dec(3));
    GPIOA->MODER |= bit2Dec(2);

    //PA2
    GPIOA->MODER &= ~(bit2Dec(5));
    GPIOA->MODER |= bit2Dec(4);

    //PA3
    GPIOA->MODER &= ~(bit2Dec(7));
    GPIOA->MODER |= bit2Dec(6);


    //Enable MODER pins for i2c
    //pb8
    GPIOB->MODER |= bit2Dec(17);
    GPIOB->MODER &= ~(bit2Dec(16));
    //pb9
    GPIOB->MODER |= bit2Dec(19);
    GPIOB->MODER &= ~(bit2Dec(20));

    //Enable output pins to power matrix columns
    //PC8
    GPIOC->MODER &= ~(bit2Dec(17));
    GPIOC->MODER |= bit2Dec(16);

    //PC9
    GPIOC->MODER &= ~(bit2Dec(19));
    GPIOC->MODER |=bit2Dec(18);

    //PC10
    GPIOC->MODER &= ~(bit2Dec(21));
    GPIOC->MODER |= bit2Dec(20);

    //PC11
    GPIOC->MODER &= ~(bit2Dec(23));
    GPIOC->MODER |= bit2Dec(22); 

    //Instatiate i2c object
    CSE321_LCD  lcd0(LCDCOL,LCDROW,LCD_5x10DOTS,PB_9,PB_8);

    

    // //Enable lcd
    lcd0.begin();
    lcd0.clear();
    lcd0.display();
    lcd0.cursor();
    lcd0.setCursor(0,0);
    lcd0.leftToRight();
    lcd0.print("HELLO CRUEL WORLD");
    lcd0.setCursor(0,1);
    lcd0.print("Please wait..");
    thread_sleep_for(5000); 
    lcd0.clear();
    
    col0.rise(&col0_handler);
    col1.rise(&col1_handler);
    col2.rise(&col2_handler);
    col3.rise(&col3_handler);
    

    printf("Entering\n");
    while (true) {
        //Cycle power thru rows
        cycleRows();
        //Print place holder for led
        if (!bufHandled){
            printf("%s\n",test);
            printf("Button pressed is %c length: %d\n",inputBuf[bufCounter],strlen(inputBuf));
            lcd0.print("*");
            switch(bufCounter){
                case 1:
                    gpioOn('A',0);
                    break;
                case 2:
                    gpioOn('A',1);
                    break;
                case 3:
                    gpioOn('A',2);
                    break;
                case 4:
                    gpioOn('A',3);
                    break;
            }
            bufHandled=true;
        }


        if (bufCounter >= PASSCODE_SIZE){
            inputBuf[bufCounter+1] = '\0';
            printf("Input Buf is: %s\n",inputBuf);
            parseInput(&lcd0);
            //Turn off all LEDs
            for (int i = 0; i < 4; i++){
                gpioOff('A',i);         //<---- Here too :)
            }
            bufCounter = 0;
            bufHandled = true;
        }
        
     
    }
}

void col3_handler(void){
    
    // Check row 0
    // Set input buffer
    // incrament buffer counter  -Do this for every single case
    if (bufHandled){
        
        if (GPIOC->ODR == (GPIOC->ODR |= bit2Dec(11))){
            inputBuf[bufCounter] ='1';
            
        }
        //check row 2
        else if (GPIOC->ODR == (GPIOC->ODR |= bit2Dec(10))){
            inputBuf[bufCounter] ='7';
            
            
        }
        //check row 1
        else if (GPIOC->ODR == (GPIOC->ODR |= bit2Dec(9))){
            inputBuf[bufCounter] ='4';
            
        }
        //check row 3
        else if(GPIOC->ODR == (GPIOC->ODR |= bit2Dec(8))){
            inputBuf[bufCounter] ='*';
            
        }
        bufHandled = false;
        bufCounter++;
    }
    return;
}
void col2_handler(void){
    if (bufHandled){
        //Check row 0
        if (GPIOC->ODR == (GPIOC->ODR |= bit2Dec(11))){
            inputBuf[bufCounter] ='2';
            
        }
        //check row 2
        else if (GPIOC->ODR == (GPIOC->ODR |= bit2Dec(10))){
            inputBuf[bufCounter] ='8';
            
            
            
        }
        //check row 1
        else if (GPIOC->ODR == (GPIOC->ODR |= bit2Dec(9))){
            inputBuf[bufCounter] ='5';
            
            
        }
        //check row 3
        else if(GPIOC->ODR == (GPIOC->ODR |= bit2Dec(8))){
            inputBuf[bufCounter] ='0';
            
        }
        bufHandled = false;
        bufCounter++;
    }
    return;
}
void col1_handler(void){
    if(bufHandled){
        //Check row 0
        if (GPIOC->ODR == (GPIOC->ODR |= bit2Dec(11))){
            inputBuf[bufCounter] ='3';
            
        }
        //check row 1
        else if (GPIOC->ODR == (GPIOC->ODR |= bit2Dec(10))){
            inputBuf[bufCounter] ='9';
            
        }
        //check row 2
        else if (GPIOC->ODR == (GPIOC->ODR |= bit2Dec(9))){
            inputBuf[bufCounter] ='6';
            
        }
        //check row 3
        else if(GPIOC->ODR == (GPIOC->ODR |= bit2Dec(8))){
            inputBuf[bufCounter] ='#';
        }
        bufHandled = false;
        bufCounter++;}
    
}
void col0_handler(void){
    if(bufHandled){
            //Check row 0
        if (GPIOC->ODR == (GPIOC->ODR |= bit2Dec(11))){
            inputBuf[bufCounter] ='A';
            
        }
        //check row 1
        else if (GPIOC->ODR == (GPIOC->ODR |= bit2Dec(10))){
            inputBuf[bufCounter] ='C';
            
            
        }
        //check row 2
        else if (GPIOC->ODR == (GPIOC->ODR |= bit2Dec(9))){
            inputBuf[bufCounter] ='B';
            
        }
        //check row 3
        else if(GPIOC->ODR == (GPIOC->ODR |= bit2Dec(8))){
            inputBuf[bufCounter] ='D';
        
        }
        bufHandled = false;
        bufCounter++;
    }
}

void setAsLocked(CSE321_LCD *lcd0){

    lcd0->setCursor(0,1);
    lcd0->print("Locked");
    lcd0->setCursor(0,0);
    thread_sleep_for(2000);
    lcd0->clear();
    //Cycle current on and onff through line of led's with a period of 1s
}

void setAsUnlocked(CSE321_LCD *lcd0){
    lcd0->setCursor(0,1);
    lcd0->print("Unlocked");
    lcd0->setCursor(0,0);
    thread_sleep_for(2000);
    lcd0->clear();
}


bool parseInput(CSE321_LCD *lcd0){
    bool out = true;

    //Check if input is a reset password command
    if (strncmp(inputBuf,"#####",4) == 0){
        //Reset password command
        resetPwd(lcd0);
    }
    else if (strncmp(inputBuf,passCode,4) == 0){
        //Unlock digital Lock
        setAsUnlocked(lcd0);
    }
    else{
        lcd0->setCursor(0,1);
        //print "Invalid password"
        lcd0->print("Invld pwd");
        //Blink LEDs 3 times
        gpioOn('A',0);
        gpioOn('A',1);
        gpioOn('A',2);
        gpioOn('A',3);
        thread_sleep_for(400);

        gpioOff('A',0);
        gpioOff('A',1);
        gpioOff('A',2);
        gpioOff('A',3);
        thread_sleep_for(400);

        gpioOn('A',0);
        gpioOn('A',1);
        gpioOn('A',2);
        gpioOn('A',3);
        thread_sleep_for(400);

        gpioOff('A',0);
        gpioOff('A',1);
        gpioOff('A',2);
        gpioOff('A',3);
        thread_sleep_for(400);

        gpioOn('A',0);
        gpioOn('A',1);
        gpioOn('A',2);
        gpioOn('A',3);
        thread_sleep_for(400);

        gpioOff('A',0);
        gpioOff('A',1);
        gpioOff('A',2);
        gpioOff('A',3);
        thread_sleep_for(400);

        gpioOn('A',0);
        gpioOn('A',1);
        gpioOn('A',2);
        gpioOn('A',3);
        thread_sleep_for(400);

        gpioOff('A',0);
        gpioOff('A',1);
        gpioOff('A',2);
        gpioOff('A',3);
        thread_sleep_for(400);

        thread_sleep_for(3000);
        lcd0->clear();
        lcd0->setCursor(0,0);
    
    }

    //Reset buffer
    for (int i = 0; i < PASSCODE_SIZE; i++){
        inputBuf[0] = '0';
    }
    return out;
}


int32_t bit2Dec(uint8_t bit){
    return (uint32_t) exp2(bit);
}

void cycleRows(void){
     //Turn on row 0PC
    GPIOC->ODR |= bit2Dec(8);
    thread_sleep_for(50);
    //Turn off row 0 PC5
    GPIOC->ODR &= ~(bit2Dec(8));
    

    
    //Turn on row 1 PC10
    GPIOC->ODR |= bit2Dec(9);
    thread_sleep_for(50);
    //Turn off row 1 PC10
    GPIOC->ODR &= ~(bit2Dec(9));
    

    //Turn on row 2 PC9
    GPIOC->ODR |= bit2Dec(10);
    thread_sleep_for(50);
    //Turn off row 2 PC9
    GPIOC->ODR &= ~(bit2Dec(10));
    

    //Turn on row 3 PC8
    GPIOC->ODR |= bit2Dec(11);
    thread_sleep_for(50);
    //Turn off row 3 PC8
    GPIOC->ODR &= ~(bit2Dec(11));
    
    
}

void resetPwd(CSE321_LCD *lcd0){
    //Print "Enter new password" on first line
    lcd0->setCursor(0,0);
    lcd0->print("Enter New Pwd:");
    lcd0->setCursor(0,1);

    //Get user input 
    for(int i = 0; i < PASSCODE_SIZE;i++){

        //Await a button press
        while(bufHandled){
            cycleRows();
        }
        //**** gets typed on second line
        lcd0->print("*");
        bufHandled = true;

    }

    thread_sleep_for(200);
    //Clear screen
    lcd0->clear();

    //Print "Pwd Change \n Success"
    lcd0->setCursor(0,0);
    lcd0->print("Pwd Change");
    lcd0->setCursor(0,1);
    lcd0->print("Success");

    //Set as locked
    thread_sleep_for(2000);
    lcd0->clear();
    setAsLocked(lcd0);
    return;

}

void gpioOn(char port, int8_t pin){
    /*  This function assumes the pin's RCC clock 
        has been enabled, and its MODER was enable 
        as a general purpose output. Should not be used 
        in ISR context
    */
    switch (port){
        case 'A':
            GPIOA->ODR |= bit2Dec(pin);
            break;
        case 'B':
            GPIOB->ODR |= bit2Dec(pin);
            break;
        case 'C':
            GPIOC->ODR |= bit2Dec(pin);
            break;
        case 'D':
            GPIOD->ODR |= bit2Dec(pin);
            break;
        case 'E':
            GPIOE->ODR |= bit2Dec(pin);
            break;
        case 'F':
            GPIOF->ODR |= bit2Dec(pin);
            break;

    }
    return;
}

void gpioOff(char port, int8_t pin){
    /*  This function assumes the pin's RCC clock 
        has been enabled, and its MODER was enable 
        as a general purpose output. Should not be used 
        in ISR context
    */
    switch (port){
        case 'A':
            GPIOA->ODR &= ~(bit2Dec(pin));
            break;
        case 'B':
            GPIOB->ODR &= ~(bit2Dec(pin));
            break;
        case 'C':
            GPIOB->ODR &= ~(bit2Dec(pin));
            break;
        case 'D':
            GPIOB->ODR &= ~(bit2Dec(pin));
            break;
        case 'E':
            GPIOB->ODR &= ~(bit2Dec(pin));
            break;
        case 'F':
            GPIOB->ODR &= ~(bit2Dec(pin));
            break;

    }
    return;
}
 




