from hypothesis import given

from tests.utils import (AlternativeNativeSetsPair,
                         are_alternative_native_sets_equal)
from . import strategies


@given(strategies.sets_pairs, strategies.sets_pairs)
def test_basic(first_pair: AlternativeNativeSetsPair,
               second_pair: AlternativeNativeSetsPair) -> None:
    first_alternative, first_native = first_pair
    second_alternative, second_native = second_pair

    first_alternative -= second_alternative
    first_native -= second_native

    assert are_alternative_native_sets_equal(first_alternative, first_native)
