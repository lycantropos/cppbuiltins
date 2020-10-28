from typing import (Any,
                    List)

from hypothesis import given

from tests.utils import (AlternativeNativeSetsPair,
                         are_alternative_native_sets_equal)
from . import strategies


@given(strategies.sets_pairs, strategies.objects_lists)
def test_basic(pair: AlternativeNativeSetsPair, other: List[Any]) -> None:
    alternative, native = pair

    alternative_result, native_result = (
        alternative.symmetric_difference_update(other),
        native.symmetric_difference_update(other))

    assert alternative_result is native_result is None
    assert are_alternative_native_sets_equal(alternative, native)
