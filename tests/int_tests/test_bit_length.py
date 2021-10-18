import sys

from hypothesis import given

from tests.utils import (AlternativeInt,
                         equivalence)
from . import strategies


@given(strategies.ints)
def test_basic(int_: AlternativeInt) -> None:
    result = int_.bit_length()

    assert isinstance(result, AlternativeInt)


@given(strategies.ints)
def test_positive_definiteness(int_: AlternativeInt) -> None:
    result = int_.bit_length()

    assert equivalence(not result, not int_)


@given(strategies.ints, strategies.ints)
def test_product(first: AlternativeInt, second: AlternativeInt) -> None:
    result = (first * second).bit_length()

    assert result <= first.bit_length() + second.bit_length()


@given(strategies.ints)
def test_evenness(int_: AlternativeInt) -> None:
    result = int_.bit_length()

    assert result == (-int_).bit_length()


@given(strategies.ints)
def test_reference_counter(int_: AlternativeInt) -> None:
    int_refcount_before = sys.getrefcount(int_)

    result = int_.bit_length()

    int_refcount_after = sys.getrefcount(int_)
    assert int_refcount_after == int_refcount_before
