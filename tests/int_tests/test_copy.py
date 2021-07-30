import copy

from hypothesis import given

from tests.utils import AlternativeInt
from . import strategies


@given(strategies.ints)
def test_shallow(int_: AlternativeInt) -> None:
    result = copy.copy(int_)

    assert isinstance(result, AlternativeInt)
    assert result is int_


@given(strategies.ints)
def test_deep(int_: AlternativeInt) -> None:
    result = copy.deepcopy(int_)

    assert isinstance(result, AlternativeInt)
    assert result is not int_
    assert result == int_
