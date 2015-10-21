#ifndef NEC_H
#define NEC_H

#include <stdint.h>

#define NEC_BURST_MIN 125
#define NEC_BURST_MAX 160

#define NEC_GAP_MIN 60
#define NEC_GAP_MAX 80

#define NEC_PULSE_MIN 6
#define NEC_PULSE_MAX 12

#define NEC_ZERO_MIN 6
#define NEC_ZERO_MAX 12

#define NEC_ONE_MIN 20
#define NEC_ONE_MAX 30

#define NEC_HOLD_MIN 30
#define NEC_HOLD_MAX 40

#define NEC_READY 0
#define NEC_REPEAT 1

void initNEC(void);

/*
 * return codes
 */
typedef enum
{
    SUCCESS,
    FAILED,
    REPEAT
} return_t;

return_t ir_burst(void);
return_t ir_gap(void);
return_t ir_address(void);
return_t ir_address_inverted(void);
return_t ir_command(void);
return_t ir_command_inverted(void);
return_t ir_repeat(void);

typedef enum
{
    BURST,
    GAP,
    ADDRESS,
    ADDRESS_INVERTED,
    COMMAND,
    COMMAND_INVERTED,
    KEY_REPEAT
} state_t;

typedef struct
{
    uint8_t address;
    uint8_t command;
    uint8_t status;
} nec_t;

extern return_t (* state[])(void);
extern state_t state_transitions[][3];
extern volatile uint8_t timer0_overflow_cnt;
extern volatile nec_t nec;

#endif /* NEC_H */
