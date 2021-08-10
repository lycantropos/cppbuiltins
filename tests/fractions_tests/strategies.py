from hypothesis import strategies

from tests.utils import (to_alternative_native_fractions_pair,
                         to_alternative_native_ints_pair)

numerators_pairs = strategies.integers().map(to_alternative_native_ints_pair)
denominators_pairs = (strategies.integers().filter(bool)
                      .map(to_alternative_native_ints_pair))
fractions_pairs = strategies.builds(to_alternative_native_fractions_pair,
                                    numerators_pairs, denominators_pairs)
