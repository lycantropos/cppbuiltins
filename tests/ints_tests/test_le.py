from hypothesis import given

from tests.utils import (AlternativeNativeIntsPair,
                         equivalence)
from . import strategies


@given(strategies.ints_pairs, strategies.ints_pairs)
def test_basic(first_pair: AlternativeNativeIntsPair,
               second_pair: AlternativeNativeIntsPair) -> None:
    alternative_first, native_first = first_pair
    alternative_second, native_second = second_pair

    assert equivalence(alternative_first <= alternative_second,
                       native_first <= native_second)
