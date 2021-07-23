from typing import Tuple

import pytest
from hypothesis import given

from tests.utils import AlternativeInt
from . import strategies


@given(strategies.decimal_int_strings)
def test_decimal_string(string: str) -> None:
    result = AlternativeInt(string)

    assert isinstance(result, AlternativeInt)


@given(strategies.int_strings_with_bases)
def test_string_with_base(string_with_base: Tuple[str, int]) -> None:
    string, base = string_with_base

    result = AlternativeInt(string, base)

    assert isinstance(result, AlternativeInt)


def test_no_argument() -> None:
    result = AlternativeInt()

    assert isinstance(result, AlternativeInt)
    assert not result


@given(strategies.strings, strategies.invalid_bases)
def test_invalid_bases(string: str, base: int) -> None:
    with pytest.raises(ValueError):
        AlternativeInt(string, base)


@given(strategies.invalid_int_strings, strategies.valid_bases)
def test_invalid_strings(string: str, base: int) -> None:
    with pytest.raises(ValueError):
        AlternativeInt(string, base)
