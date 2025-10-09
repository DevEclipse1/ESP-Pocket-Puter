#include "../protocol.h"

const Protocol protocol_linear = {
    .t_short = 500,
    .t_high = 1500,
    .t_delta = 350,
    .min_bit_count = 10
};