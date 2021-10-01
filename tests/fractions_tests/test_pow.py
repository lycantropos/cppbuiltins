from typing import Union

import pytest
from hypothesis import given

from tests.utils import (AlternativeNativeFractionsPair,
                         AlternativeNativeIntsPair,
                         are_alternative_native_fractions_equal)
from . import strategies


@given(strategies.fractions_pairs, strategies.fraction_or_int_exponents_pairs)
def test_basic(bases_pair: AlternativeNativeIntsPair,
               exponents_pair: Union[AlternativeNativeFractionsPair,
                                     AlternativeNativeIntsPair]) -> None:
    alternative_base, native_base = bases_pair
    alternative_exponent, native_exponent = exponents_pair

    try:
        alternative_result = alternative_base ** alternative_exponent
    except (ValueError, ZeroDivisionError) as error:
        with pytest.raises(type(error)):
            native_base ** native_exponent
    else:
        native_result = native_base ** native_exponent

        assert (alternative_result == native_result
                if isinstance(alternative_result, type(native_result))
                else are_alternative_native_fractions_equal(alternative_result,
                                                            native_result))
