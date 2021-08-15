import pytest
from hypothesis import given

from tests.utils import (AlternativeInt,
                         AlternativeNativeFractionsPair,
                         AlternativeNativeIntsPair,
                         are_alternative_native_fractions_equal,
                         are_alternative_native_ints_equal)
from . import strategies


@given(strategies.ints_pairs, strategies.fraction_exponents_pairs)
def test_basic(exponents_pair: AlternativeNativeIntsPair,
               bases_pair: AlternativeNativeFractionsPair) -> None:
    alternative_base, native_base = exponents_pair
    alternative_exponent, native_exponent = bases_pair

    try:
        alternative_result = alternative_base ** alternative_exponent
    except ZeroDivisionError:
        with pytest.raises(ZeroDivisionError):
            native_base ** native_exponent
    else:
        native_result = native_base ** native_exponent

        assert (alternative_result == native_result
                if isinstance(alternative_result, type(native_result))
                else
                (are_alternative_native_ints_equal(alternative_result,
                                                   native_result)
                 if isinstance(alternative_result, AlternativeInt)
                 else
                 are_alternative_native_fractions_equal(alternative_result,
                                                        native_result)))
