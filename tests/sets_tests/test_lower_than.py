from hypothesis import given

from tests.utils import (AlternativeNativeSetsPair,
                         equivalence)
from . import strategies


@given(strategies.sets_pairs, strategies.sets_pairs)
def test_basic(first_pair: AlternativeNativeSetsPair,
               second_pair: AlternativeNativeSetsPair) -> None:
    alternative_first, native_first = first_pair
    alternative_second, native_second = second_pair

    assert equivalence(alternative_first < alternative_second,
                       native_first < native_second)
