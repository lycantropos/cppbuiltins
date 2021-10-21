from hypothesis import given

from tests.utils import (AlternativeNativeIntsPair,
                         are_alternative_native_ints_equal)
from . import strategies


@given(strategies.ints_pairs, strategies.ints_pairs)
def test_basic(first_pair: AlternativeNativeIntsPair,
               second_pair: AlternativeNativeIntsPair) -> None:
    alternative_first, native_first = first_pair
    alternative_second, native_second = second_pair

    alternative_result = alternative_first | alternative_second
    native_result = native_first | native_second

    assert are_alternative_native_ints_equal(alternative_result, native_result)
