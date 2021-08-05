from hypothesis import given

from tests.utils import (AlternativeNativeIntsPair,
                         alternative_gcd,
                         are_alternative_native_ints_equal,
                         native_gcd)
from . import strategies


@given(strategies.ints_pairs, strategies.ints_pairs)
def test_basic(firsts_pair: AlternativeNativeIntsPair,
               seconds_pair: AlternativeNativeIntsPair) -> None:
    alternative_first, native_first = firsts_pair
    alternative_second, native_second = seconds_pair

    alternative_result = alternative_gcd(alternative_first, alternative_second)
    native_result = native_gcd(native_first, native_second)

    assert are_alternative_native_ints_equal(alternative_result, native_result)
