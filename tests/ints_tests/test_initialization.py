from typing import Tuple

from hypothesis import given

from tests.utils import (AlternativeInt,
                         AlternativeNativeIntsPair,
                         NativeInt,
                         are_alternative_native_ints_equal)
from . import strategies


def test_no_argument() -> None:
    alternative, native = AlternativeInt(), NativeInt()

    assert are_alternative_native_ints_equal(alternative, native)


@given(strategies.decimal_int_strings_with_leading_zeros)
def test_decimal_string(string: str) -> None:
    alternative, native = AlternativeInt(string), NativeInt(string)

    assert are_alternative_native_ints_equal(alternative, native)


@given(strategies.ints_pairs)
def test_idempotence(ints_pair: AlternativeNativeIntsPair) -> None:
    alternative_int, native_int = ints_pair

    alternative, native = (AlternativeInt(alternative_int),
                           NativeInt(native_int))

    assert are_alternative_native_ints_equal(alternative, native)


@given(strategies.int_strings_with_bases)
def test_string_with_base(string_with_base: Tuple[str, int]) -> None:
    string, base = string_with_base

    alternative, native = AlternativeInt(string, base), NativeInt(string, base)

    assert are_alternative_native_ints_equal(alternative, native)
