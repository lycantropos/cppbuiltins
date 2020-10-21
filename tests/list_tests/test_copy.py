from operator import is_

from hypothesis import given

from tests.utils import AlternativeList
from . import strategies


@given(strategies.lists)
def test_basic(list_: AlternativeList) -> None:
    result = list_.copy()

    assert isinstance(result, AlternativeList)


@given(strategies.lists)
def test_shallowness(list_: AlternativeList) -> None:
    result = list_.copy()

    assert all(map(is_, result, list_))
