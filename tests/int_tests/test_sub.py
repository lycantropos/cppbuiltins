import sys

from hypothesis import given

from tests.utils import (AlternativeInt,
                         equivalence)
from . import strategies


@given(strategies.ints, strategies.ints)
def test_basic(first: AlternativeInt, second: AlternativeInt) -> None:
    result = first - second

    assert isinstance(result, AlternativeInt)


@given(strategies.ints)
def test_diagonal(int_: AlternativeInt) -> None:
    assert not int_ - int_


@given(strategies.ints, strategies.ints)
def test_commutative_case(first: AlternativeInt,
                          second: AlternativeInt) -> None:
    assert equivalence(first - second == second - first, first == second)


@given(strategies.ints, strategies.zero_ints)
def test_right_neutral_element(first: AlternativeInt,
                               second: AlternativeInt) -> None:
    assert first - second == first


@given(strategies.ints, strategies.ints)
def test_alternatives(first: AlternativeInt, second: AlternativeInt) -> None:
    result = first - second

    assert result == first + (-second)


@given(strategies.ints, strategies.ints)
def test_reference_counter(first: AlternativeInt,
                           second: AlternativeInt) -> None:
    first_refcount_before = sys.getrefcount(first)
    second_refcount_before = sys.getrefcount(second)

    result = first - second

    first_refcount_after = sys.getrefcount(first)
    second_refcount_after = sys.getrefcount(second)
    assert first_refcount_after == first_refcount_before
    assert second_refcount_after == second_refcount_before
