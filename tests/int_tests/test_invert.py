import sys

from hypothesis import given

from tests.utils import AlternativeInt
from . import strategies


@given(strategies.ints)
def test_basic(int_: AlternativeInt) -> None:
    result = ~int_

    assert isinstance(result, AlternativeInt)


@given(strategies.ints)
def test_involution(int_: AlternativeInt) -> None:
    result = ~int_

    assert int_ == ~result


@given(strategies.ints)
def test_reference_counter(int_: AlternativeInt) -> None:
    int_refcount_before = sys.getrefcount(int_)

    result = ~int_

    int_refcount_after = sys.getrefcount(int_)
    assert int_refcount_after == int_refcount_before
