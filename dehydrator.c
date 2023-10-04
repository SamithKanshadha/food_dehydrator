#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#define LCD_DDR DDRB
#define LCD_Port PORTB
#define RS_PIN PB0
#define EN_PIN PB1

long map(int a, int input_min, int input_max, int output_min, int output_max)
{

    return (a - input_min) * (output_max - output_min) / (input_max - input_min) + output_min;
}

void ADC_Init()
{

    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    ADMUX |= (1 << REFS0);
    ADCSRA |= (1 << ADEN);
}

int ADC_Read(unsigned char chanl)
{

    ADMUX = (ADMUX & ~0b00001111) | (chanl & 0b00001111);

    ADCSRA |= (1 << ADSC);
    while (((ADCSRA >> ADSC) & 1) == 1)
    {
    }

    return ADC;
}

void LCD_Cmnd(unsigned char command)
{

    LCD_Port = (LCD_Port & 0x0F) | (command & 0xF0);
    LCD_Port &= ~(1 << RS_PIN);
    LCD_Port |= (1 << EN_PIN);
    _delay_us(1);
    LCD_Port &= ~(1 << EN_PIN);

    _delay_us(200);

    LCD_Port = (LCD_Port & 0x0F) | (command << 4);
    LCD_Port |= (1 << EN_PIN);
    _delay_us(1);
    LCD_Port &= ~(1 << EN_PIN);
    _delay_ms(2);
}

void LCD_Character(unsigned char dt)
{

    LCD_Port = (LCD_Port & 0x0F) | (dt & 0xF0);
    LCD_Port |= (1 << RS_PIN);
    LCD_Port |= (1 << EN_PIN);
    _delay_us(1);
    LCD_Port &= ~(1 << EN_PIN);

    _delay_us(200);

    LCD_Port = (LCD_Port & 0x0F) | (dt << 4);
    LCD_Port |= (1 << EN_PIN);
    _delay_us(1);
    LCD_Port &= ~(1 << EN_PIN);
    _delay_ms(2);
}

void LCD_Init(void)
{

    LCD_DDR = 0xFF;
    _delay_ms(20);

    LCD_Cmnd(0x02);
    LCD_Cmnd(0x28);
    LCD_Cmnd(0x0c);
    LCD_Cmnd(0x06);
    LCD_Cmnd(0x01);
    _delay_ms(2);
}

void LCD_S(char *str)
{

    int x;
    for (x = 0; str[x] != 0; x++)
    {
        LCD_Character(str[x]);
    }
}

void LCD_Cursor(char r, char c, char *str)
{

    if (r == 0 && c < 16)
        LCD_Cmnd((c & 0x0F) | 0x80);
    else if (r == 1 && c < 16)
        LCD_Cmnd((c & 0x0F) | 0xC0);
    LCD_S(str);
}

void LCD_Clear_Screen(void)
{

    LCD_Cmnd(0x01);
    _delay_ms(2);
    LCD_Cmnd(0x80);
}

void LCD_Convert2Str(int num)
{

    char Num_to_Str[100];
    sprintf(Num_to_Str, "%d", num);
    LCD_S(Num_to_Str);
}

unsigned short RHumid_Int;
unsigned short RHumid_Dec;
unsigned short Temp_Int;
unsigned short Temp_Dec;
unsigned short checksum;
unsigned short data_Byte = 0;

void dht_Inactive(void)
{

    DDRA &= ~(1 << PA1);
    PORTA |= (1 << PA1);
}

void dht_Begin(void)
{

    DDRA |= (1 << PA1);
    PORTA &= ~(1 << PA1);
    _delay_ms(20);
    PORTA |= (1 << PA1);
}

void dht_Reply(void)
{

    DDRA &= ~(1 << PA1);
    while (PINA & (1 << PA1))
        ;
    while ((PINA & (1 << PA1)) == 0)
        ;
    while (PINA & (1 << PA1))
        ;
}

unsigned char dht_R(void)
{

    for (unsigned char x = 0; x < 8; x++)
    {
        while ((PINA & (1 << PA1)) == 0)
            _delay_us(30);

        if (PINA & (1 << PA1))
            data_Byte = (data_Byte << 1) | (0x01);
        else
            data_Byte = (data_Byte << 1);

        while (PINA & (1 << PA1))
    }
    return data_Byte;
}
int main(void)
{

    DDRA |= (1 << PA3);

    DDRA |= (1 << PA1);
    PORTA |= (1 << PA1);

    LCD_Init();
    ADC_Init();

    while (1)
    {
        dht_Inactive();
        _delay_ms(2000);

        dht_Begin();
        dht_Reply();

        RHumid_Int = dht_R();
        RHumid_Dec = dht_R();
        Temp_Int = dht_R();
        Temp_Dec = dht_R();
        checksum = dht_R();

        int POT = ADC_Read(4);
        int POT_map = map(POT, 0, 1023, 20, 50);
        LCD_Convert2Str(POT);
        LCD_Clear_Screen();

        LCD_S("T:");
        LCD_Convert2Str(Temp_Int);
        LCD_S(" P:");
        LCD_Convert2Str(POT_map);

        LCD_Cmnd(0xC0);

        LCD_S("H:");
        LCD_Convert2Str(RHumid_Int);
        LCD_S(".");
        LCD_Convert2Str(RHumid_Dec);

        if (RHumid_Int >= POT_map)
        {
            PORTA |= (1 << PA3);
        }
        else
        {
            PORTA &= ~(1 << PA3);
        }
    }
}
#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#define LCD_DDR DDRB
#define LCD_Port PORTB
#define RS_PIN PB0
#define EN_PIN PB1

long map(int a, int input_min, int input_max, int output_min, int output_max)
{

    return (a - input_min) * (output_max - output_min) / (input_max - input_min) + output_min;
}

void ADC_Init()
{

    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    ADMUX |= (1 << REFS0);
    ADCSRA |= (1 << ADEN);
}

int ADC_Read(unsigned char chanl)
{

    ADMUX = (ADMUX & ~0b00001111) | (chanl & 0b00001111);

    ADCSRA |= (1 << ADSC);
    while (((ADCSRA >> ADSC) & 1) == 1)
    {
    }

    return ADC;
}

void LCD_Cmnd(unsigned char command)
{

    LCD_Port = (LCD_Port & 0x0F) | (command & 0xF0);
    LCD_Port &= ~(1 << RS_PIN);
    LCD_Port |= (1 << EN_PIN);
    _delay_us(1);
    LCD_Port &= ~(1 << EN_PIN);

    _delay_us(200);

    LCD_Port = (LCD_Port & 0x0F) | (command << 4);
    LCD_Port |= (1 << EN_PIN);
    _delay_us(1);
    LCD_Port &= ~(1 << EN_PIN);
    _delay_ms(2);
}

void LCD_Character(unsigned char dt)
{

    LCD_Port = (LCD_Port & 0x0F) | (dt & 0xF0);
    LCD_Port |= (1 << RS_PIN);
    LCD_Port |= (1 << EN_PIN);
    _delay_us(1);
    LCD_Port &= ~(1 << EN_PIN);

    _delay_us(200);

    LCD_Port = (LCD_Port & 0x0F) | (dt << 4);
    LCD_Port |= (1 << EN_PIN);
    _delay_us(1);
    LCD_Port &= ~(1 << EN_PIN);
    _delay_ms(2);
}

void LCD_Init(void)
{

    LCD_DDR = 0xFF;
    _delay_ms(20);

    LCD_Cmnd(0x02);
    LCD_Cmnd(0x28);
    LCD_Cmnd(0x0c);
    LCD_Cmnd(0x06);
    LCD_Cmnd(0x01);
    _delay_ms(2);
}

void LCD_S(char *str)
{

    int x;
    for (x = 0; str[x] != 0; x++)
    {
        LCD_Character(str[x]);
    }
}

void LCD_Cursor(char r, char c, char *str)
{

    if (r == 0 && c < 16)
        LCD_Cmnd((c & 0x0F) | 0x80);
    else if (r == 1 && c < 16)
        LCD_Cmnd((c & 0x0F) | 0xC0);
    LCD_S(str);
}

void LCD_Clear_Screen(void)
{

    LCD_Cmnd(0x01);
    _delay_ms(2);
    LCD_Cmnd(0x80);
}

void LCD_Convert2Str(int num)
{

    char Num_to_Str[100];
    sprintf(Num_to_Str, "%d", num);
    LCD_S(Num_to_Str);
}

unsigned short RHumid_Int;
unsigned short RHumid_Dec;
unsigned short Temp_Int;
unsigned short Temp_Dec;
unsigned short checksum;
unsigned short data_Byte = 0;

void dht_Inactive(void)
{

    DDRA &= ~(1 << PA1);
    PORTA |= (1 << PA1);
}

void dht_Begin(void)
{

    DDRA |= (1 << PA1);
    PORTA &= ~(1 << PA1);
    _delay_ms(20);
    PORTA |= (1 << PA1);
}

void dht_Reply(void)
{

    DDRA &= ~(1 << PA1);
    while (PINA & (1 << PA1))
        ;
    while ((PINA & (1 << PA1)) == 0)
        ;
    while (PINA & (1 << PA1))
        ;
}

unsigned char dht_R(void)
{

    for (unsigned char x = 0; x < 8; x++)
    {
        while ((PINA & (1 << PA1)) == 0)
            _delay_us(30);

        if (PINA & (1 << PA1))
            data_Byte = (data_Byte << 1) | (0x01);
        else
            data_Byte = (data_Byte << 1);

        while (PINA & (1 << PA1))
    }
    return data_Byte;
}
int main(void)
{

    DDRA |= (1 << PA3);

    DDRA |= (1 << PA1);
    PORTA |= (1 << PA1);

    LCD_Init();
    ADC_Init();

    while (1)
    {
        dht_Inactive();
        _delay_ms(2000);

        dht_Begin();
        dht_Reply();

        RHumid_Int = dht_R();
        RHumid_Dec = dht_R();
        Temp_Int = dht_R();
        Temp_Dec = dht_R();
        checksum = dht_R();

        int POT = ADC_Read(4);
        int POT_map = map(POT, 0, 1023, 20, 50);
        LCD_Convert2Str(POT);
        LCD_Clear_Screen();

        LCD_S("T:");
        LCD_Convert2Str(Temp_Int);
        LCD_S(" P:");
        LCD_Convert2Str(POT_map);

        LCD_Cmnd(0xC0);

        LCD_S("H:");
        LCD_Convert2Str(RHumid_Int);
        LCD_S(".");
        LCD_Convert2Str(RHumid_Dec);

        if (RHumid_Int >= POT_map)
        {
            PORTA |= (1 << PA3);
        }
        else
        {
            PORTA &= ~(1 << PA3);
        }
    }
}
