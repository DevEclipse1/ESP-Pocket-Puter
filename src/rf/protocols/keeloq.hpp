#include "../protocol.h"

const Protocol protocol_keeloq = {
    .t_short = 400,
    .t_high = 840,
    .t_delta = 140,
    .min_bit_count = 64
};