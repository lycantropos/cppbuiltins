from typing import Any

from hypothesis import given

from tests.utils import AlternativeNativeListsPair
from . import strategies


@given(strategies.lists_pairs, strategies.objects)
def test_defaults(pair: AlternativeNativeListsPair, value: Any) -> None:
    alternative, native = pair

    alternative_result, native_result = (alternative.count(value),
                                         native.count(value))

    assert alternative_result == native_result
