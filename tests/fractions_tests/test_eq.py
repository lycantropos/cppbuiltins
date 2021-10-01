from typing import Union

from hypothesis import given

from tests.utils import (AlternativeNativeFractionsPair,
                         AlternativeNativeIntsPair,
                         equivalence)
from . import strategies


@given(strategies.fractions_pairs, strategies.fractions_or_ints_pairs)
def test_basic(first_pair: AlternativeNativeFractionsPair,
               second_pair: Union[AlternativeNativeFractionsPair,
                                  AlternativeNativeIntsPair]) -> None:
    alternative_first, native_first = first_pair
    alternative_second, native_second = second_pair

    assert equivalence(alternative_first == alternative_second,
                       native_first == native_second)
