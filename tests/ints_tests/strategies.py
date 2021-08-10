from hypothesis import strategies

from tests.strategies import (decimal_int_strings_with_leading_zeros,
                              int_strings_with_bases)
from tests.utils import to_alternative_native_ints_pair

decimal_int_strings_with_leading_zeros = decimal_int_strings_with_leading_zeros
int_strings_with_bases = int_strings_with_bases
ints_pairs = strategies.integers().map(to_alternative_native_ints_pair)
