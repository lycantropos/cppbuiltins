import pytest
from hypothesis import given

from tests.utils import AlternativeNativeFractionsPair
from . import strategies


@given(strategies.fractions_pairs)
def test_basic(pair: AlternativeNativeFractionsPair) -> None:
    alternative, native = pair

    try:
        alternative_result = float(alternative)
    except OverflowError:
        with pytest.raises(OverflowError):
            float(native)
    else:
        assert alternative_result == float(native)
