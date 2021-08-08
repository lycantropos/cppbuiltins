from operator import itemgetter

from hypothesis import strategies

from tests.strategies import (decimal_int_strings_with_leading_zeros,
                              int_strings_with_bases)
from tests.utils import (pack,
                         to_alternative_native_fractions_pair,
                         to_alternative_native_ints_pair)

numerators_pairs = (strategies.builds(to_alternative_native_ints_pair,
                                      decimal_int_strings_with_leading_zeros)
                    | strategies.builds(pack(to_alternative_native_ints_pair),
                                        int_strings_with_bases))
denominators_pairs = numerators_pairs.filter(itemgetter(0))
fractions_pairs = strategies.builds(to_alternative_native_fractions_pair,
                                    numerators_pairs, denominators_pairs)
