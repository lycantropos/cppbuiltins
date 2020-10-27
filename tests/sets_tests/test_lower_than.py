from hypothesis import given

from tests.utils import (AlternativeNativeSetsPair,
                         equivalence)
from . import strategies


@given(strategies.sets_pairs, strategies.sets_pairs)
def test_basic(first_pair: AlternativeNativeSetsPair,
               second_pair: AlternativeNativeSetsPair) -> None:
    first_alternative, first_native = first_pair
    second_alternative, second_native = second_pair

    assert equivalence(first_alternative < second_alternative,
                       first_native < second_native)
