from typing import Union

import pytest
from hypothesis import given

from tests.utils import (AlternativeNativeFractionsPair,
                         AlternativeNativeIntsPair,
                         are_alternative_native_ints_equal)
from . import strategies


@given(strategies.fractions_pairs, strategies.fractions_or_ints_pairs)
def test_basic(dividends_pair: AlternativeNativeFractionsPair,
               divisors_pair: Union[AlternativeNativeFractionsPair,
                                    AlternativeNativeIntsPair]) -> None:
    alternative_dividend, native_dividend = dividends_pair
    alternative_divisor, native_divisor = divisors_pair

    try:
        alternative_result = alternative_dividend // alternative_divisor
    except ZeroDivisionError:
        with pytest.raises(ZeroDivisionError):
            native_dividend // native_divisor
    else:
        native_result = native_dividend // native_divisor

        assert are_alternative_native_ints_equal(alternative_result,
                                                 native_result)
