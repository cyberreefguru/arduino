/*
 * LetterWiring.h
 *
 *  Created on: Dec 28, 2014
 *      Author: tsasala
 */

#ifndef LETTERWIRING_H_
#define LETTERWIRING_H_

#define LETTER_P_SIZE	8
#define LETTER_O_SIZE	8
#define LETTER_W_SIZE	7
#define LETTER_E_SIZE	7
#define LETTER_R_SIZE	8
#define LETTER_T_SIZE	5

uint8_t LETTER_P1[LETTER_P_SIZE][2] = { {0,0}, {1,0}, {2,0}, {3,0}, {4,0}, {5,0}, {6,0}, {7,0} };
uint8_t LETTER_O1[LETTER_O_SIZE][2] = { {0,2}, {1,2}, {2,2}, {3,2}, {4,2}, {5,2}, {6,2}, {7,2} };
uint8_t LETTER_W1[LETTER_W_SIZE][2] = { {0,4}, {2,4}, {3,4}, {4,4}, {5,4}, {6,4}, {7,4} };
uint8_t LETTER_E1[LETTER_E_SIZE][2] = { {0,3}, {2,3}, {3,3}, {4,3}, {5,3}, {6,3}, {7,3} };
uint8_t LETTER_R1[LETTER_R_SIZE][2] = { {0,1}, {1,1}, {2,1}, {3,1}, {4,1}, {5,1}, {6,1}, {7,1} };

uint8_t LETTER_T1[LETTER_T_SIZE][2] = { {15,0}, {9,0}, {14,0}, {11,0}, {12,0} };
uint8_t LETTER_O2[LETTER_O_SIZE][2] = { {8,2}, {9,2}, {10,2}, {11,2}, {12,2}, {13,2}, {14,2}, {15,2}};
uint8_t LETTER_W2[LETTER_W_SIZE][2] = { {8,4}, {9,4}, {10,4}, {11,4}, {12,4}, {13,4}, {14,4} };
uint8_t LETTER_E2[LETTER_E_SIZE][2] = { {8,3}, {11,3}, {12,3}, {13,3}, {14,3}, {15,3}, {9,3} };
uint8_t LETTER_R2[LETTER_R_SIZE][2] = { {8,1}, {9,1}, {10,1}, {11,1}, {12,1}, {13,1}, {14,1}, {15,1} };



#endif /* LETTERWIRING_H_ */
