/********************************************************
** Samuli Piipponen ja Daniel Piippo 12.04.2019
** Pong-pelin luominen ATMega128-mikrokontrollerilla
** N�ytt� kytket��n Opetuskortti II 
**
** Alusta:
** 	Opetukortti ATMEGA 128:: AVR-GCC
**	Fuse Bits:0xFF, 0x19, 0xE4
**********************************************************/

#define F_CPU 8000000UL		
#include <util/delay.h>

#define	true	1
#define false	0
#define	NB_SHOTS	5
#define STEP_MAX	1

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include "lcd_utils.h"
#include <stdlib.h>
#include "string.h"
#include <math.h>

//Define switches as inputs
#define Switch PIND
#define SWUp PD3
#define SWDown PD1
#define SWReset PD2

// Global variables
int playerUp = 21, playerDown = 41, aiUp = 21, aiDown = 41, ballLeft = 60, ballUp = 28, ballRight = 65, ballDown = 32, directionX = 0, directionY = 0, randomi;
char scorePlayer = '0', scoreAi = '0';


int main(void){
	char Buffer[6];
	DDRD &= ~((1<<SWDown)|(1<<SWUp)|(1<<SWReset)); // Switches are inputs
	int i,j;
	
	// Starting delay
	for(i=0;i<100;i++)
		for(j=0;j<100;j++);

	LCD_Init_With_Flags(FLAG_LANDSCAPE | FLAG_IGNORE_WHITESPACE);
	
	// THE GAME
	for(;;){
		//INIT DEVICE
		LCD_Clear();
		LCD_FillRect(0, 0, 127, 2);
		// the players
		LCD_FillRect(0, playerUp, 4, playerDown); // left (human)
		LCD_FillRect(123, aiUp, 127, aiDown); // right (AI)
		// the ball
		LCD_FillRect(ballLeft, ballUp, ballRight, ballDown);

		// THE MOVEMENT OF THE BALL
		//horizontal
		if(directionX == 1){ // right
			ballRight += 3;
			ballLeft += 3;
		}
		if(directionX == 0){ // left
			ballRight -= 3;
			ballLeft -= 3;
		}
		//vertical
		if(directionY == 1 && ballUp > 2){ // up
			ballUp -= 2;
			ballDown -= 2;
		}
		if(directionY == 0){ // down
			ballUp += 0;
			ballDown += 0;
		}
		if(directionY == -1 && ballDown < 63){ // down
			ballUp += 2;
			ballDown += 2;
		}

		// If the ball misses the plane
		if(ballRight > 127){
			ballLeft = 60;
			ballUp = 28;
			ballRight = 65;
			ballDown = 32;
			directionX = 0;
			directionY = 0;
			scorePlayer++;
		}
		if(ballLeft < 0){
			ballLeft = 60;
			ballUp = 28;
			ballRight = 65;
			ballDown = 32;
			directionX = 0;
			directionY = 0;
			scoreAi++;
		}

		// Hitting the plane and returning the ball back
		if(ballRight > 121 && ballUp < aiDown && ballUp > aiUp - 5){
			directionX = 0;
		}
		if(ballLeft < 6 && ballUp < playerDown && ballUp > playerUp - 5){
			directionX = 1;
		}

		// SWITCHING THE DIRECTION OF THE BALL
		// The ball hits right in the middle of plane. It bounces horizontally (4px wide area)
		if((ballRight > 121 && ballUp < aiDown - 9 && ballUp > aiUp + 7) || (ballLeft < 6 && ballUp < playerDown - 9 && ballUp > playerUp + 7)){
			directionY = 0;
		}
		// The ball hits in the upper area of plane or the bottom of the screen. It bounces upwards (8px wide area)
		if((ballRight > 121 && ballUp < aiDown - 12 && ballUp > aiUp - 5) || (ballLeft < 6 && ballUp < playerDown - 12 && ballUp > playerUp - 5) || ballDown > 62){
			directionY = 1;
		}
		// The ball hits in the lower area of plane or the top of the screen. It bounces downwards (8px wide area)
		if((ballRight > 121 && ballUp < aiDown && ballUp > aiUp + 12) || (ballLeft < 6 && ballUp < playerDown && ballUp > playerUp + 12) || ballUp < 3){
			directionY = -1;
		}
	
		// points
		LCD_PrintChar(scorePlayer,20,4); // left
		LCD_PrintChar(scoreAi,108,4); // right
		
		// THE MOVEMENT OF THE PLAYER
		// Up
		if(~Switch & (1<<SWUp) && playerUp > 1){ // Has switch #4 been pressed
			playerDown-=2;
			playerUp-=2;
	
		}
		// Down
		if(~Switch & (1<<SWDown) && playerDown < 63){ // Has switch #2 been pressed
			playerDown+=2;
			playerUp+=2;
		}


				
		// THE MOVEMENT OF THE AI
		if (rand() % 11 > 3){
			if(ballDown < aiUp + 10 && aiUp > 1){ // Up
				aiDown-=2;
				aiUp-=2;
			}
			if(ballDown > aiUp + 10 && aiDown < 63){ // Down
				aiDown+=2;
				aiUp+=2;
			}
		}


		// Print on screen
		LCD_Display();


		// The victory screens
		if(scorePlayer >= '3'){
			sprintf(Buffer, "Voitit");
			LCD_PrintString(Buffer, 50, 30);
			LCD_Display();
			return 0;
		}
		if(scoreAi >= '3'){
			sprintf(Buffer, "Havisit");
			LCD_PrintString(Buffer, 45, 30);
			LCD_Display();
			return 0;
		}
	}
}
	

