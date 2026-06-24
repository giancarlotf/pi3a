#define __SFR_OFFSET 0x00
#include <avr/io.h>
#include <avr/interrupt.h>

; Atraso com Timer0 (Polling)
.global delay_us
.global delay_ms

; Cronômetro com Timer2
.global time_start
.global time_stop
.global time_count ; retorna tempo atual e zera a variável temporária

.global TIM2_COMPA

; Atraso
delay_us:
    push r16
    push r17
    in r16, SREG
    push r16

    ldi r16, 0x10
    ldi r17, 0x01   ; (1 << CS00)
    rjmp set_timer0

delay_ms:
    push r16
    push r17
    in r16, SREG
    push r16

    ldi r16, 0xFA
    ldi r17, 0x03   ; (1 << CS01)|(1 << CS00)
    rjmp set_timer0

set_timer0:
    out OCR0A, r16
    ldi r16, 0x02
    out TCCR0A, r16 ; (1 << WGM01): Modo CTC com valor máximo = OCR0A
    out TIFR0, r16  ; (1 << OCF0A)
    clr R16
    out TCNT0, r16  ; Zera o registrador de temporização
    out TCCR0B, r17

    sts TIMSK0, r16

loop_delay:
    in r16, TIFR0
    sbrs r16, OCF0A
    rjmp loop_delay

    ldi r16, 0x02
    out TIFR0, r16  ; (1 << OCF0A)

    dec r24
    breq return
    rjmp loop_delay

return:
    clr r16
    out TCCR0B, r16

    pop r16
    out SREG, r16
    pop r17
    pop r16
    ret

; Cronômetro
time_start:
    ;a
    sei
    ret

time_count:
    ;ldi r24, value
    ret

time_stop:
    cli
    ret