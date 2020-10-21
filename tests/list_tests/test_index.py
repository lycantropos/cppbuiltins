from typing import (Any,
                    Tuple)

import pytest
from hypothesis import given

from tests.utils import AlternativeNativeListsPair
from . import strategies


@given(strategies.non_empty_lists_pairs_with_their_elements)
def test_defaults(pair_with_value: Tuple[AlternativeNativeListsPair, Any]
                  ) -> None:
    (alternative, native), value = pair_with_value

    alternative_result, native_result = (alternative.index(value),
                                         native.index(value))

    assert alternative_result == native_result


@given(strategies.non_empty_lists_pairs_with_starts_stops_and_their_elements)
def test_full(pair_with_start_stop_and_value
              : Tuple[AlternativeNativeListsPair, int, int, Any]) -> None:
    (alternative, native), start, stop, value = pair_with_start_stop_and_value

    alternative_result, native_result = (alternative.index(value, start, stop),
                                         native.index(value, start, stop))

    assert alternative_result == native_result


@given(strategies.lists_pairs_with_non_their_elements)
def test_defaults_missing(pair_with_value
                          : Tuple[AlternativeNativeListsPair, Any]) -> None:
    (alternative, native), value = pair_with_value

    with pytest.raises(ValueError):
        alternative.index(value)
    with pytest.raises(ValueError):
        native.index(value)


@given(strategies.lists_pairs_with_starts_stops_and_non_their_elements)
def test_full_missing(pair_with_value
                      : Tuple[AlternativeNativeListsPair, int, int, Any]
                      ) -> None:
    (alternative, native), start, stop, value = pair_with_value

    with pytest.raises(ValueError):
        alternative.index(value, start, stop)
    with pytest.raises(ValueError):
        native.index(value, start, stop)
