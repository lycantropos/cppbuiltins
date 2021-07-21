import sys

from hypothesis import given

from tests.utils import AlternativeInt
from . import strategies


@given(strategies.ints)
def test_round_trip(int_: AlternativeInt) -> None:
    result = repr(int_)

    assert eval(result, sys.modules) == int_
