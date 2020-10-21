from hypothesis import given

from tests.utils import (AlternativeNativeListsPair,
                         are_iterators_equal)
from . import strategies


@given(strategies.lists_pairs)
def test_basic(pair: AlternativeNativeListsPair) -> None:
    alternative, native = pair

    alternative_result, native_result = reversed(alternative), reversed(native)

    assert are_iterators_equal(alternative_result, native_result)
