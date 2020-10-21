from hypothesis import given

from tests.utils import (AlternativeNativeListsPair,
                         are_alternative_native_lists_equal)
from . import strategies


@given(strategies.lists_pairs)
def test_basic(pair: AlternativeNativeListsPair) -> None:
    alternative, native = pair

    alternative_result = alternative.copy()
    native_result = native.copy()

    assert are_alternative_native_lists_equal(alternative_result,
                                              native_result)
