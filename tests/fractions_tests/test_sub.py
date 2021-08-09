from hypothesis import given

from tests.utils import (AlternativeNativeFractionsPair,
                         are_alternative_native_fractions_equal)
from . import strategies


@given(strategies.fractions_pairs, strategies.fractions_pairs)
def test_basic(minuends_pair: AlternativeNativeFractionsPair,
               subtrahends_pair: AlternativeNativeFractionsPair) -> None:
    alternative_minuend, native_minuend = minuends_pair
    alternative_subtrahend, native_subtrahend = subtrahends_pair

    alternative_result = alternative_minuend - alternative_subtrahend
    native_result = native_minuend - native_subtrahend

    assert are_alternative_native_fractions_equal(alternative_result,
                                                  native_result)
