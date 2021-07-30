from hypothesis import given

from tests.utils import (AlternativeInt,
                         pickle_round_trip)
from . import strategies


@given(strategies.ints)
def test_round_trip(int_: AlternativeInt) -> None:
    result = pickle_round_trip(int_)

    assert isinstance(result, AlternativeInt)
    assert result is not int_
    assert result == int_
