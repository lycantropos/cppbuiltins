from hypothesis import given

from tests.utils import (AlternativeNativeSetsPair,
                         are_alternative_native_sets_equal)
from . import strategies


@given(strategies.sets_pairs)
def test_basic(pair: AlternativeNativeSetsPair) -> None:
    alternative, native = pair

    alternative_result = alternative.copy()
    native_result = native.copy()

    assert are_alternative_native_sets_equal(alternative_result,
                                             native_result)
