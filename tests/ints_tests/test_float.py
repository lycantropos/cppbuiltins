import pytest
from hypothesis import given

from tests.utils import AlternativeNativeIntsPair
from . import strategies


@given(strategies.ints_pairs)
def test_basic(pair: AlternativeNativeIntsPair) -> None:
    alternative, native = pair

    try:
        alternative_result = float(alternative)
    except OverflowError:
        with pytest.raises(OverflowError):
            float(native)
    else:
        assert alternative_result == float(native)
