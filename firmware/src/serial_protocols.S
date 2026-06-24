; Códigos presentes no datasheet do ATmega328P, disponibilizado pela Microchip Technology (Atmel), com pequenas alterações.

#define __SFR_OFFSET 0x00
#include <avr/io.h>

; TWI (I2C/IIC)
.global TWI_init
.global TWI_start
.global TWI_write
.global TWI_read_ack
.global TWI_read_nack
.global TWI_stop

; USART
.global USART_init
.global USART_transmit
.global USART_receive
.global USART_flush

; TWI (I2C/IIC)
TWI_init:
    ldi r16, 0x00
    sts TWSR, r16
    ldi r16, 0x48
    sts TWBR, r16
    ldi r16, (1<<TWEN)
    sts TWCR, r16
    ret

TWI_start:
    ; Envia a condição de START
    ldi r16, (1<<TWINT) | (1<<TWSTA) | (1<<TWEN)
    sts TWCR, r16

wait1:
    ; Espera pela flag TWINT ser definida.
    ; Isso indica que a condição de START foi transmitida
    lds r16, TWCR
    sbrs r16, TWINT
    rjmp wait1
    ret

TWI_write:
    ; Carrega SLA_W dentro do registrador TWDR
    ; Limpa o bit TWINT no TWCR para começar a transmissão do endereço
    sts TWDR, r24
    ldi r24, (1<<TWINT) | (1<<TWEN)
    sts TWCR, r24

wait2:
    ; Espera pela flag TWINT ser definida.
    ; Isso indica que SLA+W foi transmitido e ACK/NACK foi recebido
    lds r24, TWCR
    sbrs r24, TWINT
    rjmp wait2
    ret

TWI_read_ack:
    ; Carrega DATA dentro do registrador TWDR
    ; Limpa o bit TWINT no TWCR para começar a transmissão de dados
    ldi r16, (1<<TWINT) | (1<<TWEN) | (1<<TWEA)
    sts TWCR, r16

wait3:
    ; Espera pela flag TWINT ser definida.
    ; Isso indica que DATA foi transmitido e ACK/NACK foi recebido
    lds r16, TWCR
    sbrs r16, TWINT
    rjmp wait3

    lds r24, TWDR
    ret

TWI_read_nack:
    ; Carrega DATA dentro do registrador TWDR
    ; Limpa o bit TWINT no TWCR para começar a transmissão de dados
    ldi r16, (1<<TWINT) | (1<<TWEN)
    sts TWCR, r16

wait4:
    ; Espera pela flag TWINT ser definida.
    ; Isso indica que DATA foi transmitido e ACK/NACK foi recebido
    lds r16, TWCR
    sbrs r16, TWINT
    rjmp wait4

    lds r24, TWDR
    ret

TWI_stop:
    ; Transmite a condição de STOP
    ldi r16, (1<<TWINT) | (1<<TWSTO) | (1<<TWEN)
    sts TWCR, r16 
    ret

; USART
USART_init:
    ; Define taxa de transmissão
    sts UBRR0H, r25
    sts UBRR0L, r24
    ; Habilita receptor e transmissor
    ldi r16, (1<<RXEN0) | (1<<TXEN0)
    sts UCSR0B, r16
    ; Define o formato de quadro: 8data, 2stop bitbaud rate
    ldi r16, (1 << UCSZ01) | (1 << UCSZ00)
    sts UCSR0C, r16
    ret

USART_transmit:
    ; Aguarda por um buffer de transmissão vazio
    lds r16, UCSR0A
    sbrs r16, UDRE0
    rjmp USART_transmit
    ; Coloca os dados (r24) no buffer e envia os dados
    sts UDR0, r24
    ret

USART_receive:
    ; Aguarda pelos dados a serem recebidos
    lds r16, UCSR0A
    sbrs r16, RXC0
    rjmp USART_receive
    ; Pega e retorna os dados recebidos do buffer
    lds r24, UDR0
    ret

USART_flush:
    lds r16, UCSR0A
    sbrs r16, RXC0
    ret
    lds r16, UDR0
    rjmp USART_flush