import string
import sys

from hypothesis import strategies

from tests.strategies import (decimal_int_strings_with_leading_zeros,
                              int_strings_with_bases)
from tests.utils import (AlternativeInt,
                         pack)

strings = strategies.text()
invalid_bases = strategies.just(1) | strategies.integers(37, sys.maxsize)
invalid_int_strings = strategies.text(strategies.characters(
        blacklist_categories=['Nd'],
        blacklist_characters=string.ascii_letters))
valid_bases = strategies.just(0) | strategies.integers(2, 36)
ints = (strategies.builds(AlternativeInt,
                          decimal_int_strings_with_leading_zeros)
        | strategies.builds(pack(AlternativeInt), int_strings_with_bases))
unary_ints = strategies.builds(AlternativeInt, strategies.just('1'),
                               strategies.sampled_from(range(2, 37)))
zero_ints = strategies.builds(AlternativeInt)
