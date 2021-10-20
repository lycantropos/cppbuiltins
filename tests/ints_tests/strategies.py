import sys

from hypothesis import strategies

from tests.strategies import (decimal_int_strings_with_leading_zeros,
                              int_strings_with_bases)
from tests.utils import to_alternative_native_ints_pair

decimal_int_strings_with_leading_zeros = decimal_int_strings_with_leading_zeros
int_strings_with_bases = int_strings_with_bases
ints_pairs = strategies.integers().map(to_alternative_native_ints_pair)
non_zero_ints_pairs = (strategies.integers().filter(bool)
                       .map(to_alternative_native_ints_pair))
single_byte_ints_pairs = (strategies.integers(-128, 127)
                          .map(to_alternative_native_ints_pair))
exponents_with_moduli_pairs = (
        strategies.tuples(single_byte_ints_pairs,
                          strategies.tuples(strategies.none(),
                                            strategies.none()))
        | strategies.tuples((strategies.integers(0)
                             if sys.version_info < (3, 8)
                             else strategies.integers())
                            .map(to_alternative_native_ints_pair),
                            non_zero_ints_pairs))
