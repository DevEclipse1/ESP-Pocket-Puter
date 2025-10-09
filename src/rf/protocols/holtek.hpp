#include "../protocol.h"

const Protocol protocol_holtek = {
    .t_short = 430,
    .t_high = 870,
    .t_delta = 100,
    .min_bit_count = 40
};