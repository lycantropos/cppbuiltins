from hypothesis import given

from tests.utils import (AlternativeNativeSetsPair,
                         are_alternative_native_sets_equal,
                         pickle_round_trip)
from . import strategies


@given(strategies.sets_pairs)
def test_basic(pair: AlternativeNativeSetsPair) -> None:
    alternative, native = pair

    assert are_alternative_native_sets_equal(pickle_round_trip(alternative),
                                             pickle_round_trip(native))
