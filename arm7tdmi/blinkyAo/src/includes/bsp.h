/*
BSP Include for BlinkyAO - QM framework active object port of
Keil Blinky demo program for MCB 2300
bsp function references for standalone port of QM FSM 7 HSM 
*/

				


#ifndef  bsp_h
#define  bsp_h



/*  Board specific functions */

void Init_Timer1(void );
void lcd_print (unsigned char const *string);
void lcd_clear (void);
void set_cursor (unsigned char column, unsigned char line);
void lcd_init (void);
void init_ad( void);
void    Init_Timer1(void );
void    init_serial(void );                               /* Init UART                   */
void    uart_init_0(void );
void    _sys_exit(int);

void Disp_Bargraph(int pos_x, int pos_y, int value);

#define _exit(x)  _sys_exit(x)



void    delay( int time );

int  _kbhit(void);
int   kbhit(void);
int  _getch(void);
int   getch(void);
void _sys_exit(int);


/* bsp function references for blinky_ao Active Object */

void BSP_Init(void);
void BSP_Start_AD(void);

void BSP_Display_Serial(short AD_value) ;
void BSP_Display_LED(int val );
void BSP_Display_LCD(int pos, int row, int val);









#endif

