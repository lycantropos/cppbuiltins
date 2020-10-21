from hypothesis import given

from tests.utils import (AlternativeNativeListsPair,
                         are_alternative_native_lists_equal)
from . import strategies


@given(strategies.lists_pairs, strategies.lists_pairs)
def test_basic(first_pair: AlternativeNativeListsPair,
               second_pair: AlternativeNativeListsPair) -> None:
    first_alternative, first_native = first_pair
    second_alternative, second_native = second_pair

    alternative_result = first_alternative + second_alternative
    native_result = first_native + second_native

    assert are_alternative_native_lists_equal(alternative_result,
                                              native_result)
