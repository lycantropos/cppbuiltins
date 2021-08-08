from hypothesis import given

from tests.utils import (AlternativeFraction,
                         AlternativeNativeIntsPair,
                         NativeFraction,
                         are_alternative_native_fractions_equal)
from . import strategies


@given(strategies.numerators_pairs, strategies.denominators_pairs)
def test_basic(numerators_pair: AlternativeNativeIntsPair,
               denominators_pair: AlternativeNativeIntsPair) -> None:
    alternative_numerator, native_numerator = numerators_pair
    alternative_denominator, native_denominator = denominators_pair

    alternative, native = (AlternativeFraction(alternative_numerator,
                                               alternative_denominator),
                           NativeFraction(native_numerator,
                                          native_denominator))

    assert are_alternative_native_fractions_equal(alternative, native)
