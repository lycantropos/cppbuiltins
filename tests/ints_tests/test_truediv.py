import pytest
from hypothesis import given

from tests.utils import AlternativeNativeIntsPair
from . import strategies


@given(strategies.ints_pairs, strategies.ints_pairs)
def test_basic(dividends_pair: AlternativeNativeIntsPair,
               divisors_pair: AlternativeNativeIntsPair) -> None:
    alternative_dividend, native_dividend = dividends_pair
    alternative_divisor, native_divisor = divisors_pair

    try:
        alternative_result = alternative_dividend / alternative_divisor
    except (OverflowError, ZeroDivisionError) as error:
        with pytest.raises(type(error)):
            native_dividend / native_divisor
    else:
        native_result = native_dividend / native_divisor

        assert alternative_result == native_result
