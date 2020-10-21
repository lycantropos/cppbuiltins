from hypothesis import given

from tests.utils import (AlternativeNativeListsPair,
                         are_alternative_native_lists_equal)
from . import strategies


@given(strategies.lists_pairs)
def test_basic(pair: AlternativeNativeListsPair) -> None:
    alternative, native = pair

    alternative_result, native_result = alternative.reverse(), native.reverse()

    assert alternative_result == native_result
    assert are_alternative_native_lists_equal(alternative, native)
