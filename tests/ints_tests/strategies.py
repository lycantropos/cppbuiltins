from operator import itemgetter

from hypothesis import strategies

from tests.strategies import (decimal_int_strings_with_leading_zeros,
                              int_strings_with_bases)
from tests.utils import (pack,
                         to_alternative_native_ints_pair)

ints_pairs = (strategies.builds(to_alternative_native_ints_pair,
                                decimal_int_strings_with_leading_zeros)
              | strategies.builds(pack(to_alternative_native_ints_pair),
                                  int_strings_with_bases))
non_zero_ints_pairs = ints_pairs.filter(itemgetter(0))
