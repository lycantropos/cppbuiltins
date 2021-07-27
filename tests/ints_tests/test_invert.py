from hypothesis import given

from tests.utils import (AlternativeNativeIntsPair,
                         are_alternative_native_ints_equal)
from . import strategies


@given(strategies.ints_pairs)
def test_basic(pair: AlternativeNativeIntsPair) -> None:
    alternative, native = pair

    assert are_alternative_native_ints_equal(~alternative, ~native)
