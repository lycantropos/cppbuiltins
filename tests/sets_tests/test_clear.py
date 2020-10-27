from hypothesis import given

from tests.utils import (AlternativeNativeSetsPair,
                         are_alternative_native_sets_equal)
from . import strategies


@given(strategies.sets_pairs)
def test_basic(pair: AlternativeNativeSetsPair) -> None:
    alternative, native = pair

    alternative_result, native_result = alternative.clear(), native.clear()

    assert alternative_result is native_result is None
    assert are_alternative_native_sets_equal(alternative, native)
