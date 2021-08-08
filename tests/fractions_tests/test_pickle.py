from hypothesis import given

from tests.utils import (AlternativeNativeFractionsPair,
                         are_alternative_native_fractions_equal,
                         pickle_round_trip)
from . import strategies


@given(strategies.fractions_pairs)
def test_basic(pair: AlternativeNativeFractionsPair) -> None:
    alternative, native = pair

    assert are_alternative_native_fractions_equal(
            pickle_round_trip(alternative), pickle_round_trip(native))
