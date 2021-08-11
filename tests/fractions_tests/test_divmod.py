import pytest
from hypothesis import given

from tests.utils import (AlternativeNativeFractionsPair,
                         are_alternative_native_fractions_equal,
                         are_alternative_native_ints_equal)
from . import strategies


@given(strategies.fractions_pairs, strategies.fractions_pairs)
def test_basic(dividends_pair: AlternativeNativeFractionsPair,
               divisors_pair: AlternativeNativeFractionsPair) -> None:
    alternative_dividend, native_dividend = dividends_pair
    alternative_divisor, native_divisor = divisors_pair

    try:
        alternative_quotient, alternative_remainder = divmod(
                alternative_dividend, alternative_divisor)
    except ZeroDivisionError:
        with pytest.raises(ZeroDivisionError):
            divmod(native_dividend, native_divisor)
    else:
        native_quotient, native_remainder = divmod(native_dividend,
                                                   native_divisor)

        assert are_alternative_native_ints_equal(alternative_quotient,
                                                 native_quotient)
        assert are_alternative_native_fractions_equal(alternative_remainder,
                                                      native_remainder)
