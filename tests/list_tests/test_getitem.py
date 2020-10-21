from typing import Tuple

import pytest
from hypothesis import given

from tests.utils import (AlternativeNativeListsPair,
                         are_alternative_native_lists_equal)
from . import strategies


@given(strategies.non_empty_lists_pairs_with_their_indices)
def test_index(pair_with_index: Tuple[AlternativeNativeListsPair, int]
               ) -> None:
    (alternative, native), index = pair_with_index

    alternative_result, native_result = alternative[index], native[index]

    assert alternative_result == native_result


@given(strategies.lists_pairs_with_invalid_indices)
def test_invalid_index(pair_with_index: Tuple[AlternativeNativeListsPair, int]
                       ) -> None:
    (alternative, native), index = pair_with_index

    with pytest.raises(IndexError):
        alternative[index]
    with pytest.raises(IndexError):
        native[index]


@given(strategies.lists_pairs_with_slices)
def test_slice(pair_with_slice: Tuple[AlternativeNativeListsPair, int]
               ) -> None:
    (alternative, native), slice_ = pair_with_slice

    alternative_result, native_result = alternative[slice_], native[slice_]

    assert are_alternative_native_lists_equal(alternative_result,
                                              native_result)
