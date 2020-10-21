from typing import (Any,
                    List)

from hypothesis import given

from tests.utils import (AlternativeNativeListsPair,
                         are_alternative_native_lists_equal)
from . import strategies


@given(strategies.lists_pairs, strategies.objects_lists)
def test_basic(pair: AlternativeNativeListsPair, values: List[Any]) -> None:
    alternative, native = pair

    alternative += values
    native += values

    assert are_alternative_native_lists_equal(alternative, native)
