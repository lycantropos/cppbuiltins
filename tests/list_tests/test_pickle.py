from hypothesis import given

from tests.utils import (AlternativeNativeListsPair,
                         are_alternative_native_lists_equal,
                         pickle_round_trip)
from . import strategies


@given(strategies.non_empty_lists_pairs)
def test_basic(pair: AlternativeNativeListsPair) -> None:
    alternative, native = pair

    assert are_alternative_native_lists_equal(pickle_round_trip(alternative),
                                              pickle_round_trip(native))
