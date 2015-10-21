#include "NEC.h"

#include "UART.h"
#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t bit_counter = 0;
volatile uint8_t tmp = 0;
volatile uint8_t address = 0;
volatile uint8_t command = 0;
volatile uint8_t timer0_overflow_cnt = 0;
volatile state_t cur_state = BURST;
volatile return_t rc;
volatile nec_t nec;

return_t (* state[])(void) = {ir_burst, ir_gap, ir_address, ir_address_inverted, ir_command, ir_command_inverted, ir_repeat};

/*
 * State transition table
 */
state_t  state_transitions[][3] = {
    [BURST] = {GAP, BURST, BURST},
    [GAP] = {ADDRESS, BURST, KEY_REPEAT},
    [ADDRESS] = {ADDRESS_INVERTED, BURST, ADDRESS},
    [ADDRESS_INVERTED] = {COMMAND, BURST, ADDRESS_INVERTED},
    [COMMAND] = {COMMAND_INVERTED, BURST, COMMAND},
    [COMMAND_INVERTED] = {BURST, BURST, COMMAND_INVERTED},
    [KEY_REPEAT] = {BURST, BURST, KEY_REPEAT}
};

void initNEC(void)
{
    // enable INT0 on every state change
    EICRA |= (1 << ISC00);
    EIMSK |= (1 << INT0);

    // set PD2 as input
    DDRD &= ~(1 << DDD2);

    // timer0 8 bit config
    // prescaler 1024
    // enable overflow interrupt
    TCCR0B |= (1 << CS02 | 1 << CS00);
    TIMSK0 |= (1 << TOIE0);
}

inline return_t ir_decode(state_t state)
{
    uint8_t pinState = (PIND & (1 << PD2));
    uint8_t time = TCNT0;

    if (pinState)
    {
        // reset timer
        TCNT0 = 0;

        if ((time > NEC_PULSE_MIN) && (time < NEC_PULSE_MAX))
        {
            // return ok
            return REPEAT;
        }
        return FAILED;
    }
    else
    {
        // reset timer
        TCNT0 = 0;

        // check for zero or one as address
        if ((time > NEC_ZERO_MIN) && (time < NEC_ZERO_MAX))
        {
            if (state == ADDRESS || state == COMMAND)
            {
                tmp &= ~(1 << bit_counter++);
            }
            else
            {
                if (!(tmp & (1 << bit_counter++)))
                {
                    return FAILED;
                }
            }
        }
        else if ((time > NEC_ONE_MIN) && (time < NEC_ONE_MAX))
        {
            if (state == ADDRESS || state == COMMAND)
            {
                tmp |= (1 << bit_counter++);
            }
            else
            {
                if (tmp & (1 << bit_counter++))
                {
                    return FAILED;
                }
            }
        }

        if (bit_counter >= 8)
        {
            switch(state)
            {
                case ADDRESS:
                    address = tmp;
                    break;
                case COMMAND:
                    command = tmp;
                    break;
            }
            bit_counter = 0;
            return SUCCESS;
        }
    }

    return REPEAT;
}

return_t ir_burst(void)
{
    uint8_t pinState = (PIND & (1 << PD2));
    uint8_t time = TCNT0;

    // reset timer
    TCNT0 = 0;

    if (pinState && (time > NEC_BURST_MIN) && (time < NEC_BURST_MAX))
    {
        // return ok
        return SUCCESS;
    }

    return FAILED;
}

return_t ir_gap(void)
{
    uint8_t pinState = (PIND & (1 << PD2));
    uint8_t time = TCNT0;

    // reset timer
    TCNT0 = 0;

    if (!pinState)
    {
        if ((time > NEC_GAP_MIN) && (time < NEC_GAP_MAX))
        {
            // return ok
            return SUCCESS;
        }
        else if((time > NEC_HOLD_MIN) && (time < NEC_HOLD_MAX))
        {
            return REPEAT;
        }
    }

    return FAILED;
}

return_t ir_address(void)
{
    return ir_decode(ADDRESS);
}

return_t ir_address_inverted(void)
{
    return ir_decode(ADDRESS_INVERTED);
}

return_t ir_command(void)
{
    return ir_decode(COMMAND);
}

return_t ir_command_inverted(void)
{
    return_t returnCode = ir_decode(COMMAND_INVERTED);
    if ((returnCode == SUCCESS) && !(nec.status & (1 << NEC_READY)))
    {
        nec.address = address;
        nec.command = command;
        nec.status |= (1 << NEC_READY);
    }
    return returnCode;
}

return_t ir_repeat(void)
{
    nec.status |= (1 << NEC_REPEAT);
    // reset counter for waiting 114.24ms
    timer0_overflow_cnt = 7;
    return SUCCESS;
}

ISR(INT0_vect)
{
    rc = state[cur_state]();
    cur_state = state_transitions[cur_state][rc];
}

ISR(TIMER0_OVF_vect)
{
    // overflows every 16.32ms
    // each tick is 64us long
    if (timer0_overflow_cnt > 0)
    {
        timer0_overflow_cnt--;
    }
    if (timer0_overflow_cnt == 0)
    {
        nec.status &= ~(1 << NEC_REPEAT);
    }
}
