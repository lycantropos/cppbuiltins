import sys

from hypothesis import given

from tests.utils import AlternativeSet
from . import strategies


@given(strategies.sets)
def test_basic(set_: AlternativeSet) -> None:
    result = repr(set_)

    assert result.startswith(AlternativeSet.__module__)
    assert AlternativeSet.__qualname__ in result


@given(strategies.sets)
def test_round_trip(set_: AlternativeSet) -> None:
    result = repr(set_)

    assert eval(result, sys.modules) == set_
