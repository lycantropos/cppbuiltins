import pytest
from hypothesis import given

from tests.utils import (AlternativeNativeIntsPair,
                         are_alternative_native_ints_equal)
from . import strategies


@given(strategies.ints_pairs, strategies.single_byte_ints_pairs)
def test_basic(dividends_pair: AlternativeNativeIntsPair,
               divisors_pair: AlternativeNativeIntsPair) -> None:
    alternative_dividend, native_dividend = dividends_pair
    alternative_divisor, native_divisor = divisors_pair

    try:
        alternative_result = alternative_dividend << alternative_divisor
    except (MemoryError, OverflowError, ValueError) as error:
        with pytest.raises(type(error)):
            native_dividend << native_divisor
    else:
        native_result = native_dividend << native_divisor

        assert are_alternative_native_ints_equal(alternative_result,
                                                 native_result)
