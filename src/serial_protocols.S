; Códigos presentes no datasheet do ATmega328P, disponibilizado pela Microchip Technology (Atmel), com pequenas alterações.

#define __SFR_OFFSET 0x00
#include <avr/io.h>

; TWI (I2C/IIC)
.global TWI_Init
.global TWI_Start
.global TWI_Write
.global TWI_Read
.global TWI_Stop

; USART
.global USART_Init
.global USART_Transmit
.global USART_Receive
.global USART_Flush

; TWI (I2C/IIC)
TWI_Init:
    ldi r16, 0x00
    out TWSR, r16
    ldi r16, 0x48
    out TWBR, r16
    ldi r16, (1<<TWEN)
    out TWCR, r16
    ret

TWI_Start:
    ; Send START condition
    ldi r16, (1<<TWINT) | (1<<TWSTA) | (1<<TWEN)
    out TWCR, r16

wait1:
    ; Wait for TWINT flag set
    ; This indicates that the START condition has been transmitted
    in r16,TWCR
    sbrs r16,TWINT
    rjmp wait1
    ret

TWI_Write:
    ; Load SLA_W into TWDR register
    ; Clear TWINT bit in TWCR to start transmission of address
    ; ldi r16, SLA_W
    out TWDR, r24
    ldi r24, (1<<TWINT) | (1<<TWEN)
    out TWCR, r24

wait2:
    ; Wait for TWINT flag set
    ; This indicates that the SLA+W has been transmitted, and ACK/NACK has been received
    in r24, TWCR
    sbrs r24, TWINT
    rjmp wait2
    ret

TWI_Read:
    ; Load DATA into TWDR register
    ; Clear TWINT bit in TWCR to start transmission of data
    ldi r16, DATA
    out TWDR, r16
    ldi r16, (1<<TWINT) | (1<<TWEN)
    out TWCR, r16

wait3:
    ; Wait for TWINT flag set.
    ; This indicates that the DATA has been transmitted, and ACK/NACK has been received
    in r16, TWCR
    sbrs r16, TWINT
    rjmp wait3
    ret

TWI_Stop:
    ; Transmit STOP condition
    ldi r16, (1<<TWINT) | (1<<TWEN) | (1<<TWSTO)
    out TWCR, r16 
    ret

; USART
USART_Init:
    ; Define taxa de transmissão
    out UBRR0H, r25
    out UBRR0L, r24
    ; Habilita receptor e transmissor
    ldi r16, (1<<RXEN0) | (1<<TXEN0)
    out UCSR0B, r16
    ; Define o formato de quadro: 8data, 2stop bitbaud rate
    ldi r16, (1<<USBS0) | (3<<UCSZ00)
    out UCSR0C, r16
    ret

USART_Transmit:
    ; Aguarda por um buffer de transmissão vazio
    sbis UCSR0A, UDRE0
    rjmp USART_Transmit
    ; Coloca os dados (r24) no buffer e envia os dados
    out UDR0, r24
    ret

USART_Receive:
    ; Aguarda pelos dados a serem recebidos
    sbis UCSR0A, RXC0
    rjmp USART_Receive
    ; Pega e retorna os dados recebidos do buffer
    in r24, UDR0
    ret

USART_Flush:
    sbis UCSR0A, RXC0
    ret
    in r16, UDR0
    rjmp USART_Flush
