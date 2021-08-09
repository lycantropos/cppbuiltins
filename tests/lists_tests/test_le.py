from hypothesis import given

from tests.utils import (AlternativeNativeListsPair,
                         equivalence)
from . import strategies


@given(strategies.lists_pairs, strategies.lists_pairs)
def test_basic(first_pair: AlternativeNativeListsPair,
               second_pair: AlternativeNativeListsPair) -> None:
    alternative_first, native_first = first_pair
    alternative_second, native_second = second_pair

    assert equivalence(alternative_first <= alternative_second,
                       native_first <= native_second)
