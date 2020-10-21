from hypothesis import given

from tests.utils import (AlternativeNativeListsPair,
                         equivalence)
from . import strategies


@given(strategies.lists_pairs, strategies.lists_pairs)
def test_basic(first_pair: AlternativeNativeListsPair,
               second_pair: AlternativeNativeListsPair) -> None:
    first_alternative, first_native = first_pair
    second_alternative, second_native = second_pair

    assert equivalence(first_alternative <= second_alternative,
                       first_native <= second_native)
