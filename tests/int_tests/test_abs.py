import sys

from hypothesis import given

from tests.utils import (AlternativeInt,
                         equivalence)
from . import strategies


@given(strategies.ints)
def test_basic(int_: AlternativeInt) -> None:
    result = abs(int_)

    assert isinstance(result, AlternativeInt)


@given(strategies.ints)
def test_idempotence(int_: AlternativeInt) -> None:
    result = abs(int_)

    assert result == abs(result)


@given(strategies.ints)
def test_positive_definiteness(int_: AlternativeInt) -> None:
    result = abs(int_)

    assert equivalence(not result, not int_)


@given(strategies.ints)
def test_evenness(int_: AlternativeInt) -> None:
    result = abs(int_)

    assert result == abs(-int_)


@given(strategies.ints, strategies.ints)
def test_multiplicativity(first: AlternativeInt,
                          second: AlternativeInt) -> None:
    result = abs(first * second)

    assert result == abs(first) * abs(second)


@given(strategies.ints, strategies.ints)
def test_triangle_inequality(first: AlternativeInt,
                             second: AlternativeInt) -> None:
    result = abs(first + second)

    assert result <= abs(first) + abs(second)


@given(strategies.ints)
def test_reference_counter(int_: AlternativeInt) -> None:
    int_refcount_before = sys.getrefcount(int_)

    result = abs(int_)

    int_refcount_after = sys.getrefcount(int_)
    assert int_refcount_after == int_refcount_before
