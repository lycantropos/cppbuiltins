from hypothesis import given

from tests.utils import (AlternativeNativeFractionsPair,
                         equivalence)
from . import strategies


@given(strategies.fractions_pairs, strategies.fractions_pairs)
def test_basic(first_pair: AlternativeNativeFractionsPair,
               second_pair: AlternativeNativeFractionsPair) -> None:
    alternative_first, native_first = first_pair
    alternative_second, native_second = second_pair

    assert equivalence(alternative_first <= alternative_second,
                       native_first <= native_second)
