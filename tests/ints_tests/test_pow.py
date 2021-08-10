from typing import (Tuple,
                    Union)

import pytest
from hypothesis import given

from tests.utils import (AlternativeNativeIntsPair,
                         are_alternative_native_ints_equal)
from . import strategies


@given(strategies.ints_pairs, strategies.exponents_with_moduli_pairs)
def test_basic(bases_pair: AlternativeNativeIntsPair,
               exponents_pair_with_moduli_pair
               : Tuple[AlternativeNativeIntsPair,
                       Union[Tuple[None, None], AlternativeNativeIntsPair]]
               ) -> None:
    alternative_base, native_base = bases_pair
    exponents_pair, moduli_pair = exponents_pair_with_moduli_pair
    alternative_exponent, native_exponent = exponents_pair
    alternative_modulus, native_modulus = moduli_pair

    try:
        alternative_result = pow(alternative_base, alternative_exponent,
                                 alternative_modulus)
    except (ZeroDivisionError, ValueError) as error:
        with pytest.raises(type(error)):
            pow(native_base, native_exponent, native_modulus)
    else:
        native_result = pow(native_base, native_exponent, native_modulus)

        assert (alternative_result == native_result
                if isinstance(alternative_result, float)
                else are_alternative_native_ints_equal(alternative_result,
                                                       native_result))
