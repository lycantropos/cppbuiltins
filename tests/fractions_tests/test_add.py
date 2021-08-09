from hypothesis import given

from tests.utils import (AlternativeNativeFractionsPair,
                         are_alternative_native_fractions_equal)
from . import strategies


@given(strategies.fractions_pairs, strategies.fractions_pairs)
def test_basic(first_pair: AlternativeNativeFractionsPair,
               second_pair: AlternativeNativeFractionsPair) -> None:
    alternative_first, native_first = first_pair
    alternative_second, native_second = second_pair

    alternative_result = alternative_first + alternative_second
    native_result = native_first + native_second

    assert are_alternative_native_fractions_equal(alternative_result,
                                                  native_result)
