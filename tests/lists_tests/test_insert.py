from typing import Any

from hypothesis import given

from tests.utils import (AlternativeNativeListsPair,
                         are_alternative_native_lists_equal)
from . import strategies


@given(strategies.lists_pairs, strategies.indices, strategies.objects)
def test_basic(pair: AlternativeNativeListsPair,
               index: int,
               value: Any) -> None:
    alternative, native = pair

    alternative_result, native_result = (alternative.insert(index, value),
                                         native.insert(index, value))

    assert alternative_result is native_result is None
    assert are_alternative_native_lists_equal(alternative, native)
