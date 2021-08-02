from hypothesis import given

from tests.utils import (AlternativeNativeIntsPair,
                         are_alternative_native_ints_equal)
from . import strategies


@given(strategies.ints_pairs, strategies.non_zero_ints_pairs)
def test_basic(dividends_pair: AlternativeNativeIntsPair,
               divisors_pair: AlternativeNativeIntsPair) -> None:
    alternative_dividend, native_dividend = dividends_pair
    alternative_divisor, native_divisor = divisors_pair

    alternative_quotient, alternative_remainder = divmod(alternative_dividend,
                                                         alternative_divisor)
    native_quotient, native_remainder = divmod(native_dividend, native_divisor)

    assert are_alternative_native_ints_equal(alternative_quotient,
                                             native_quotient)
    assert are_alternative_native_ints_equal(alternative_remainder,
                                             native_remainder)
