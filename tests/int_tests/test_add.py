import sys
from numbers import Rational

from hypothesis import given

from tests.utils import AlternativeInt
from . import strategies


@given(strategies.ints, strategies.ints)
def test_basic(first: AlternativeInt, second: Rational) -> None:
    result = first + second

    assert isinstance(result, AlternativeInt)


@given(strategies.ints, strategies.ints)
def test_commutativity(first: AlternativeInt, second: AlternativeInt) -> None:
    assert first + second == second + first


@given(strategies.ints, strategies.zero_ints)
def test_neutral_element(first: AlternativeInt,
                         second: AlternativeInt) -> None:
    assert first + second == first == second + first


@given(strategies.ints, strategies.ints, strategies.ints)
def test_associativity(first: AlternativeInt,
                       second: AlternativeInt,
                       third: AlternativeInt) -> None:
    assert (first + second) + third == first + (second + third)


@given(strategies.ints, strategies.ints)
def test_reference_counter(first: AlternativeInt,
                           second: AlternativeInt) -> None:
    first_refcount_before = sys.getrefcount(first)
    second_refcount_before = sys.getrefcount(second)

    result = first + second

    first_refcount_after = sys.getrefcount(first)
    second_refcount_after = sys.getrefcount(second)
    assert first_refcount_after == first_refcount_before
    assert second_refcount_after == second_refcount_before
