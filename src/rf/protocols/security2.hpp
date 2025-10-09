#include "../protocol.h"

const Protocol protocol_security2 = {
    .t_short = 250,
    .t_high = 500,
    .t_delta = 250,
    .min_bit_count = 62
};