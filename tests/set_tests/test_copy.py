from operator import is_

from hypothesis import given

from tests.utils import AlternativeSet
from . import strategies


@given(strategies.sets)
def test_basic(set_: AlternativeSet) -> None:
    result = set_.copy()

    assert isinstance(result, AlternativeSet)


@given(strategies.sets)
def test_shallowness(set_: AlternativeSet) -> None:
    result = set_.copy()

    assert all(map(is_, result, set_))
