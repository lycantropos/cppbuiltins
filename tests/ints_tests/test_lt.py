from hypothesis import given

from tests.utils import (AlternativeNativeIntsPair,
                         equivalence)
from . import strategies


@given(strategies.ints_pairs, strategies.ints_pairs)
def test_basic(first_pair: AlternativeNativeIntsPair,
               second_pair: AlternativeNativeIntsPair) -> None:
    first_alternative, first_native = first_pair
    second_alternative, second_native = second_pair

    assert equivalence(first_alternative < second_alternative,
                       first_native < second_native)
