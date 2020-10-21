import sys

from hypothesis import given

from tests.utils import AlternativeList
from . import strategies


@given(strategies.lists)
def test_basic(list_: AlternativeList) -> None:
    result = repr(list_)

    assert result.startswith(AlternativeList.__module__)
    assert AlternativeList.__qualname__ in result


@given(strategies.lists)
def test_round_trip(list_: AlternativeList) -> None:
    result = repr(list_)

    assert eval(result, sys.modules) == list_
