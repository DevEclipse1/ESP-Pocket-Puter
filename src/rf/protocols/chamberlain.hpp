#include "../protocol.h"

const Protocol protocol_chamberlain = {
    .t_short = 1000,
    .t_high = 3000,
    .t_delta = 200,
    .min_bit_count = 10
};