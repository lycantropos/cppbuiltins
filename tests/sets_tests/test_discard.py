from typing import Any

from hypothesis import given

from tests.utils import (AlternativeNativeSetsPair,
                         are_alternative_native_sets_equal)
from . import strategies


@given(strategies.sets_pairs, strategies.objects)
def test_basic(pair: AlternativeNativeSetsPair, value: Any) -> None:
    alternative, native = pair

    alternative_result = alternative.discard(value)
    native_result = native.discard(value)

    assert alternative_result is native_result is None
    assert are_alternative_native_sets_equal(alternative, native)
