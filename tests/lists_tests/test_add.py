from hypothesis import given

from tests.utils import (AlternativeNativeListsPair,
                         are_alternative_native_lists_equal)
from . import strategies


@given(strategies.lists_pairs, strategies.lists_pairs)
def test_basic(first_pair: AlternativeNativeListsPair,
               second_pair: AlternativeNativeListsPair) -> None:
    alternative_first, native_first = first_pair
    alternative_second, native_second = second_pair

    alternative_result = alternative_first + alternative_second
    native_result = native_first + native_second

    assert are_alternative_native_lists_equal(alternative_result,
                                              native_result)
