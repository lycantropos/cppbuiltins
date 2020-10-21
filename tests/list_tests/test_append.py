from typing import Any

from hypothesis import given

from tests.utils import (AlternativeNativeListsPair,
                         are_alternative_native_lists_equal)
from . import strategies


@given(strategies.non_empty_lists_pairs, strategies.objects)
def test_basic(pair: AlternativeNativeListsPair, value: Any) -> None:
    alternative, native = pair

    alternative_result, native_result = (alternative.append(value),
                                         native.append(value))

    assert alternative_result is native_result is None
    assert are_alternative_native_lists_equal(alternative, native)
