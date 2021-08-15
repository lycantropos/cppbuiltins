import math

from hypothesis import given

from tests.utils import (AlternativeNativeFractionsPair,
                         are_alternative_native_ints_equal)
from . import strategies


@given(strategies.fractions_pairs)
def test_basic(pair: AlternativeNativeFractionsPair) -> None:
    alternative, native = pair

    assert are_alternative_native_ints_equal(math.ceil(alternative),
                                             math.ceil(native))
