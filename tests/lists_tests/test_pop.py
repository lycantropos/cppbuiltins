from typing import Tuple

import pytest
from hypothesis import given

from tests.utils import (AlternativeNativeListsPair,
                         are_alternative_native_lists_equal)
from . import strategies


@given(strategies.non_empty_lists_pairs)
def test_defaults(pair: AlternativeNativeListsPair) -> None:
    alternative, native = pair

    alternative_result, native_result = alternative.pop(), native.pop()

    assert alternative_result == native_result
    assert are_alternative_native_lists_equal(alternative, native)


@given(strategies.non_empty_lists_pairs_with_indices)
def test_full(pair_with_index: Tuple[AlternativeNativeListsPair, int]) -> None:
    (alternative, native), index = pair_with_index

    alternative_result, native_result = (alternative.pop(index),
                                         native.pop(index))

    assert alternative_result == native_result
    assert are_alternative_native_lists_equal(alternative, native)


@given(strategies.lists_pairs_with_invalid_indices)
def test_invalid_index(pair_with_index: Tuple[AlternativeNativeListsPair, int]
                       ) -> None:
    (alternative, native), index = pair_with_index

    with pytest.raises(IndexError):
        alternative.pop(index)
    with pytest.raises(IndexError):
        native.pop(index)
