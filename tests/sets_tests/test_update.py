from typing import (Any,
                    Set)

from hypothesis import given

from tests.utils import (AlternativeNativeSetsPair,
                         are_alternative_native_sets_equal)
from . import strategies


@given(strategies.sets_pairs, strategies.objects_lists)
def test_basic(pair: AlternativeNativeSetsPair, values: Set[Any]) -> None:
    alternative, native = pair

    alternative_result, native_result = (alternative.update(values),
                                         native.update(values))

    assert alternative_result is native_result is None
    assert are_alternative_native_sets_equal(alternative, native)
