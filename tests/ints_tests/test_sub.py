from hypothesis import given

from tests.utils import (AlternativeNativeIntsPair,
                         are_alternative_native_ints_equal)
from . import strategies


@given(strategies.ints_pairs, strategies.ints_pairs)
def test_basic(minuends_pair: AlternativeNativeIntsPair,
               subtrahends_pair: AlternativeNativeIntsPair) -> None:
    alternative_minuend, native_minuend = minuends_pair
    alternative_subtrahend, native_subtrahend = subtrahends_pair

    alternative_result = alternative_minuend - alternative_subtrahend
    native_result = native_minuend - native_subtrahend

    assert are_alternative_native_ints_equal(alternative_result, native_result)
