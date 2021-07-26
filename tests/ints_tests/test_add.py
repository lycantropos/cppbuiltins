from hypothesis import given

from tests.utils import (AlternativeNativeIntsPair,
                         are_alternative_native_ints_equal)
from . import strategies


@given(strategies.ints_pairs, strategies.ints_pairs)
def test_basic(first_pair: AlternativeNativeIntsPair,
               second_pair: AlternativeNativeIntsPair) -> None:
    first_alternative, first_native = first_pair
    second_alternative, second_native = second_pair

    alternative_result = first_alternative + second_alternative
    native_result = first_native + second_native

    assert are_alternative_native_ints_equal(alternative_result, native_result)
