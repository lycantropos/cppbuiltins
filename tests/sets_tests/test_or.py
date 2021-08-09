from hypothesis import given

from tests.utils import (AlternativeNativeSetsPair,
                         are_alternative_native_sets_equal)
from . import strategies


@given(strategies.sets_pairs, strategies.sets_pairs)
def test_basic(first_pair: AlternativeNativeSetsPair,
               second_pair: AlternativeNativeSetsPair) -> None:
    alternative_first, native_first = first_pair
    alternative_second, native_second = second_pair

    alternative_result = alternative_first | alternative_second
    native_result = native_first | native_second

    assert are_alternative_native_sets_equal(alternative_result, native_result)
