from hypothesis import given

from tests.utils import (AlternativeNativeListsPair,
                         are_alternative_native_lists_equal)
from . import strategies


@given(strategies.lists_pairs)
def test_basic(pair: AlternativeNativeListsPair) -> None:
    alternative, native = pair

    alternative_result, native_result = alternative.clear(), native.clear()

    assert alternative_result is native_result is None
    assert are_alternative_native_lists_equal(alternative, native)
