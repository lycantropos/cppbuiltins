from hypothesis import strategies

from tests.utils import (to_alternative_native_fractions_pair,
                         to_alternative_native_ints_pair)

ints_pairs = strategies.integers().map(to_alternative_native_ints_pair)
non_zero_ints_pairs = (strategies.integers().filter(bool)
                       .map(to_alternative_native_ints_pair))
fractions_pairs = strategies.builds(to_alternative_native_fractions_pair,
                                    ints_pairs, non_zero_ints_pairs)
fractions_or_ints_pairs = ints_pairs | fractions_pairs
int_exponents_pairs = strategies.integers(-100, 100).map(
        to_alternative_native_ints_pair)
fraction_exponents_pairs = (int_exponents_pairs
                            .map(to_alternative_native_fractions_pair))
integral_exponents_pairs = int_exponents_pairs | fraction_exponents_pairs
